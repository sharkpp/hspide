#include "stdafx.h"
#include <windows.h>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <spplib/apihook.h>

//-------------------------------------------------------------------
// �֐��錾

void InstallAPIHookCore(HMODULE handle);

//-------------------------------------------------------------------
// �^�錾

// �t�@�C�����
typedef struct {
	std::string			filename;	// �t�@�C����
	DWORD				offset;		// �������݈ʒu
	std::vector<BYTE>	data;		// �������݃f�[�^
	bool				userdef_as;
} FILE_INFO;

//-------------------------------------------------------------------
// �ϐ���`

bool g_bRedirectMode = false;
bool g_bHSPideFix = false;

// �t�@�C�����
std::map<HANDLE, FILE_INFO> g_mapFileInfo;

// �W���n���h���`�F�b�N�p���
std::set<HANDLE> g_setStdXXX;

// �t�b�N���s��API�̃I���W�i����ۑ����Ă����ϐ�
static HANDLE  (WINAPI *  CreateFileA_)(LPCTSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE) = NULL;
static BOOL    (WINAPI *  CloseHandle_)(HANDLE) = NULL;
static BOOL    (WINAPI *    WriteFile_)(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED) = NULL;
static BOOL    (WINAPI *    ReadFile_)(HANDLE,LPVOID,DWORD,LPDWORD,LPOVERLAPPED) = NULL;
static HMODULE (WINAPI * LoadLibraryA_)(LPCTSTR) = NULL;
static DWORD   (WINAPI * SetFilePointer_)(HANDLE,LONG,PLONG,DWORD) = NULL;

//-------------------------------------------------------------------
// API�t�b�N�p�֐�����

HANDLE WINAPI HookCreateFileA(
					LPCTSTR lpFileName,                         // �t�@�C����
					DWORD dwDesiredAccess,                      // �A�N�Z�X���[�h
					DWORD dwShareMode,                          // ���L���[�h
					LPSECURITY_ATTRIBUTES lpSecurityAttributes, // �Z�L�����e�B�L�q�q
					DWORD dwCreationDisposition,                // �쐬���@
					DWORD dwFlagsAndAttributes,                 // �t�@�C������
					HANDLE hTemplateFile                        // �e���v���[�g�t�@�C���̃n���h��
				)
{
	TCHAR filename[MAX_PATH*2+1];

	std::string hook_filename;
	bool redirect_mine = false;
	bool userdef_as	= g_bHSPideFix && !lstrcmpi(lpFileName, "userdef.as");

	if( g_bRedirectMode )
	{
		if( dwDesiredAccess & GENERIC_WRITE )
		{
			hook_filename = lpFileName;
			// �_�~�[���ɏ�������
			::GetModuleFileName(NULL, filename, MAX_PATH*2);
			lpFileName				= filename;
			dwDesiredAccess			= 0;
			dwCreationDisposition	= OPEN_EXISTING;
		}
	}

	// �I���W�i����API���Ăяo��
	HANDLE r = CreateFileA_(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,
	                        dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);

	if( 0 != (dwDesiredAccess & GENERIC_READ) &&
		userdef_as )
	{
		hook_filename = lpFileName;
		// ������x�J���Ȃ���
		if( INVALID_HANDLE_VALUE == r )
		{
			// �_�~�[���ɏ�������
			::GetModuleFileName(NULL, filename, MAX_PATH*2);
			lpFileName				= filename;
			dwDesiredAccess			= 0;
			dwCreationDisposition	= OPEN_EXISTING;
			redirect_mine			= true;
			// ������x�Ăяo��
			r = CreateFileA_(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,
							 dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
		}
	}

	if( INVALID_HANDLE_VALUE != r &&
		!hook_filename.empty() )
	{
		// I/O�������t�b�N���邽�߂̏���o�^
		FILE_INFO & info = g_mapFileInfo[r];
		info.filename	= hook_filename;
		info.offset		= 0;
		info.userdef_as	= userdef_as;

		if( info.userdef_as )
		{
			if( !redirect_mine ) {
				DWORD size = SetFilePointer_(r,0,NULL,FILE_END);
				if( (DWORD)-1 != size ) {
					info.data.resize(size);
					DWORD read;
					SetFilePointer_(r,0,NULL,FILE_BEGIN);
					ReadFile_(r, &info.data[0], size, &read, NULL);
					read = read;
				}
			}
			static const char INJECTION_USERDEF_AS[] = 
				"#regcmd \"_hspdbg_init@4\",\"hsp3debug.dll\"\r\n"
				"#cmd hspdbg_load $000\r\n"
				"hspdbg_load\r\n";
			info.data.insert(
					info.data.begin(),
					INJECTION_USERDEF_AS,
					INJECTION_USERDEF_AS + sizeof(INJECTION_USERDEF_AS) - 1
				);
		}
	}

	return r;
}

BOOL WINAPI HookCloseHandle(
					HANDLE hObject   // �I�u�W�F�N�g�̃n���h��
				)
{
	// �W������/�o��/�G���[�̃n���h���ł͂Ȃ�����
	if( g_setStdXXX.end() == g_setStdXXX.find(hObject) )
	{
		// �Ǘ��ς݂̏��ɑ��݂��邱��
		std::map<HANDLE, FILE_INFO>::iterator
			ite = g_mapFileInfo.find(hObject);
		if( g_mapFileInfo.end() != ite )
		{
			FILE_INFO& fi = ite->second;
			// �t�@�C�����ƃT�C�Y�Ɠ��e�������o��
			if( !fi.data.empty() &&
				!fi.userdef_as )
			{
				BOOL (WINAPI *pfnOrig)(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED) = 
					  (BOOL (WINAPI *)(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED))
						GetProcAddress( GetModuleHandleA(_T("kernel32.dll")), _T("WriteFile") );
				DWORD dwWrite, dwLen = fi.data.size();
				pfnOrig(GetStdHandle(STD_ERROR_HANDLE), fi.filename.c_str(), fi.filename.size() + 1, &dwWrite, NULL);
				pfnOrig(GetStdHandle(STD_ERROR_HANDLE), _T("\r\n"), 2, &dwWrite, NULL);
				pfnOrig(GetStdHandle(STD_ERROR_HANDLE), &dwLen, sizeof(dwLen), &dwWrite, NULL);
				pfnOrig(GetStdHandle(STD_ERROR_HANDLE), &fi.data[0], fi.data.size(), &dwWrite, NULL);
			}
			g_mapFileInfo.erase(ite);
		}
	}

	// �I���W�i����API���Ăяo��
	BOOL r = CloseHandle_(hObject);

	return r;
}

BOOL WINAPI HookWriteFile(
					HANDLE hFile,                    // �t�@�C���̃n���h��
					LPCVOID lpBuffer,                // �f�[�^�o�b�t�@
					DWORD nNumberOfBytesToWrite,     // �������ݑΏۂ̃o�C�g��
					LPDWORD lpNumberOfBytesWritten,  // �������񂾃o�C�g��
					LPOVERLAPPED lpOverlapped        // �I�[�o�[���b�v�\���̂̃o�b�t�@
				)
{
	// �W������/�o��/�G���[�̃n���h���ł͂Ȃ�����
	if( g_setStdXXX.end() == g_setStdXXX.find(hFile) )
	{
		// �Ǘ��ς݂̏��ɑ��݂��邱��
		std::map<HANDLE, FILE_INFO>::iterator
			ite = g_mapFileInfo.find(hFile);
		if( g_mapFileInfo.end() != ite )
		{
			FILE_INFO& fi = ite->second;
			// �������ݓ��e���t�@�C���ɏ������Ƀ������ɕێ�
			fi.data.resize(fi.offset + nNumberOfBytesToWrite);
			if( !fi.data.empty() ) {
				::memcpy(&fi.data[0] + fi.offset, lpBuffer, nNumberOfBytesToWrite);
			}
			fi.offset += nNumberOfBytesToWrite;
			return TRUE;
		}
	}

	// �I���W�i����API���Ăяo��
	BOOL r = WriteFile_(hFile,lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten,lpOverlapped);

	return r;
}

BOOL WINAPI HookReadFile(
					HANDLE hFile,                // �t�@�C���̃n���h��
					LPVOID lpBuffer,             // �f�[�^�o�b�t�@
					DWORD nNumberOfBytesToRead,  // �ǂݎ��Ώۂ̃o�C�g��
					LPDWORD lpNumberOfBytesRead, // �ǂݎ�����o�C�g��
					LPOVERLAPPED lpOverlapped    // �I�[�o�[���b�v�\���̂̃o�b�t�@
				)
{
	// �W������/�o��/�G���[�̃n���h���ł͂Ȃ�����
	if( g_setStdXXX.end() == g_setStdXXX.find(hFile) )
	{
		// �Ǘ��ς݂̏��ɑ��݂��邱��
		std::map<HANDLE, FILE_INFO>::iterator
			ite = g_mapFileInfo.find(hFile);
		if( g_mapFileInfo.end() != ite )
		{
			FILE_INFO& fi = ite->second;
			// �ʂ̓��e��}���������e�������o��
			if( fi.userdef_as )
			{
				DWORD read = (std::min)(nNumberOfBytesToRead, fi.data.size() - fi.offset);
				::memcpy(lpBuffer, &fi.data[0], read);
				*lpNumberOfBytesRead = read;
				return TRUE;
			}
		}
	}

	// �I���W�i����API���Ăяo��
	BOOL r = ReadFile_(hFile,lpBuffer,nNumberOfBytesToRead,lpNumberOfBytesRead,lpOverlapped);

	// �Ǘ�����Ă���Ώۂ��H
	if( FALSE != r && g_bHSPideFix )
	{
		// #regcmd 18 ��T���ď�������
		static const char   SEARCH_STR_REGCMD[] = "#regcmd ";
		static const size_t SEARCH_LEN_REGCMD = sizeof(SEARCH_STR_REGCMD) - 1; // '\0'�̕��������炷
		char*  pszBuffer = reinterpret_cast<char*>(lpBuffer);
		size_t lenBuffer = lpNumberOfBytesRead ? *lpNumberOfBytesRead : 0;
		for(char* p = pszBuffer, *last = pszBuffer + lenBuffer;
			p + SEARCH_LEN_REGCMD < last; )
		{
			char* next = (char*)::memchr(p, SEARCH_STR_REGCMD[0], size_t(last - p));
			if( !next || last <= next + SEARCH_LEN_REGCMD ) {
				break;
			}
			if( !::memcmp(next, SEARCH_STR_REGCMD, SEARCH_LEN_REGCMD) )
			{
				char* prev = pszBuffer == next ? next : next - 1;
				for(; pszBuffer < prev && ' ' == *prev || '\t' == *prev; prev--);
				if( pszBuffer == prev ||
					'\r' == *prev ||
					'\n' == *prev )
				{
					char* param_start = next + SEARCH_LEN_REGCMD;
					char* param_end = last;
					if( char* nextSP = (char*)::memchr(param_start, ' ',  size_t(last - param_start)) ) {
						param_end = param_end < nextSP ? param_end : nextSP;
					}
					if( char* nextTAB = (char*)::memchr(param_start, '\t',  size_t(last - param_start)) ) {
						param_end = param_end < nextTAB ? param_end : nextTAB;
					}
					if( char* nextComment = (char*)::memchr(param_start, ';',  size_t(last - param_start)) ) {
						param_end = param_end < nextComment ? param_end : nextComment;
					}
					if( char* nextCr = (char*)::memchr(param_start, '\r', size_t(last - param_start)) ) {
						param_end = param_end < nextCr ? param_end : nextCr;
					}
					if( char* nextLf = (char*)::memchr(param_start, '\n', size_t(last - param_start)) ) {
						param_end = param_end < nextLf ? param_end : nextLf;
					}
					if( 2 == size_t(param_end - param_start) &&
						'1' == param_start[0] &&
						'8' == param_start[1] )
					{
						param_start[0] = '1';
						param_start[1] = '9';
					}
					p = param_end;
					continue;
				}
			}
			p = next + SEARCH_LEN_REGCMD;
		}
	}

	return r;
}

DWORD WINAPI HookSetFilePointer(
					HANDLE hFile,                // �t�@�C���̃n���h��
					LONG lDistanceToMove,        // �|�C���^���ړ�����ׂ��o�C�g��
					PLONG lpDistanceToMoveHigh,  // �|�C���^���ړ�����ׂ��o�C�g��
					DWORD dwMoveMethod           // �J�n�_
				)
{
	// �W������/�o��/�G���[�̃n���h���ł͂Ȃ�����
	if( g_setStdXXX.end() == g_setStdXXX.find(hFile) )
	{
		// �Ǘ��ς݂̏��ɑ��݂��邱��
		std::map<HANDLE, FILE_INFO>::iterator
			ite = g_mapFileInfo.find(hFile);
		if( g_mapFileInfo.end() != ite )
		{
			FILE_INFO& fi = ite->second;
			//
			if( fi.userdef_as )
			{
				switch(dwMoveMethod) {
				case FILE_BEGIN:
					fi.offset = lDistanceToMove;
					break;
				case FILE_CURRENT:
					fi.offset += lDistanceToMove;
					break;
				case FILE_END:
					fi.offset = fi.data.size() - lDistanceToMove;
					break;
				}
				return fi.offset;
			}
		}
	}

	// �I���W�i����API���Ăяo��
	DWORD r = SetFilePointer_(hFile,lDistanceToMove,lpDistanceToMoveHigh,dwMoveMethod);

	return r;
}

HMODULE WINAPI HookLoadLibraryA(
					LPCTSTR lpFileName   // ���W���[���̃t�@�C����
				)
{
	// �I���W�i����API���Ăяo��
	HMODULE r = LoadLibraryA_(lpFileName);

	// �ǂݍ���DLL�Ŏg�p���Ă���API�Ƀt�b�N���d�|����
	InstallAPIHookCore(r);

	return r;
}

//-------------------------------------------------------------------
// �֐�����

void InstallAPIHook(bool redirect, bool hspide_fix)
{
	g_bRedirectMode = redirect;
	g_bHSPideFix    = hspide_fix;

	if( !g_bRedirectMode && !g_bHSPideFix ) {
		return;
	}

	// �I���W�i����API��ۑ�
	if( !LoadLibraryA_ )
	{
		LoadLibraryA_	= LoadLibraryA;
		CreateFileA_	= CreateFileA;
		CloseHandle_	= CloseHandle;
		WriteFile_		= WriteFile;
		ReadFile_		= ReadFile;
		SetFilePointer_	= SetFilePointer;

		// �W�����o�̓n���h�����r�̂��߂ɕۑ�
		g_setStdXXX.insert(GetStdHandle(STD_INPUT_HANDLE));
		g_setStdXXX.insert(GetStdHandle(STD_OUTPUT_HANDLE));
		g_setStdXXX.insert(GetStdHandle(STD_ERROR_HANDLE));
	}

	// API�t�b�N
	InstallAPIHookCore(NULL);
}

void InstallAPIHookCore(HMODULE handle)
{
	INSTALL_HOOK(_T("kernel32.dll"), _T("LoadLibraryA"),	HookLoadLibraryA,	handle);
	INSTALL_HOOK(_T("kernel32.dll"), _T("CreateFileA"),		HookCreateFileA,	handle);
	INSTALL_HOOK(_T("kernel32.dll"), _T("CloseHandle"),		HookCloseHandle,	handle);
	INSTALL_HOOK(_T("kernel32.dll"), _T("WriteFile"),		HookWriteFile,		handle);
	INSTALL_HOOK(_T("kernel32.dll"), _T("ReadFile"),		HookReadFile,		handle);
	INSTALL_HOOK(_T("kernel32.dll"), _T("SetFilePointer"),	HookSetFilePointer,	handle);
}

void UninstallAPIHook()
{
	if( !g_bRedirectMode && !g_bHSPideFix ) {
		return;
	}
	UNINSTALL_HOOK(_T("kernel32.dll"), _T("LoadLibraryA"),	HookLoadLibraryA,	NULL);
	UNINSTALL_HOOK(_T("kernel32.dll"), _T("CreateFileA"),	HookCreateFileA,	NULL);
	UNINSTALL_HOOK(_T("kernel32.dll"), _T("CloseHandle"),	HookCloseHandle,	NULL);
	UNINSTALL_HOOK(_T("kernel32.dll"), _T("WriteFile"),		HookWriteFile,		NULL);
	UNINSTALL_HOOK(_T("kernel32.dll"), _T("ReadFile"),		HookReadFile,		NULL);
	UNINSTALL_HOOK(_T("kernel32.dll"), _T("SetFilePointer"),HookSetFilePointer,	NULL);
}

