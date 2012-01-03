#include "stdafx.h"
#include <windows.h>
#include <map>
#include <vector>
#include <string>
#include "apihook.h"

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
} FILE_INFO;

//-------------------------------------------------------------------
// �ϐ���`

// �t�@�C�����
std::map<HANDLE, FILE_INFO> g_mapFileInfo;

// �t�b�N���s��API�̃I���W�i����ۑ����Ă����ϐ�
static HANDLE  (WINAPI *  CreateFileA_)(LPCTSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE) = NULL;
static BOOL    (WINAPI *  CloseHandle_)(HANDLE) = NULL;
static BOOL    (WINAPI *    WriteFile_)(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED) = NULL;
static HMODULE (WINAPI * LoadLibraryA_)(LPCTSTR) = NULL;

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
	if( dwDesiredAccess & GENERIC_WRITE )
	{
		hook_filename = lpFileName;
		// �_�~�[���ɏ�������
		::GetModuleFileName(NULL, filename, MAX_PATH*2);
		lpFileName				= filename;
		dwDesiredAccess			= 0;
		dwCreationDisposition	= OPEN_EXISTING;
	}

	// �I���W�i����API���Ăяo��
	HANDLE r = CreateFileA_(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,
	                        dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);

	if( !hook_filename.empty() )
	{
		// I/O�������t�b�N���邽�߂̏���o�^
		FILE_INFO & info = g_mapFileInfo[r];
		info.filename	= hook_filename;
		info.offset		= 0;
	}

	return r;
}

BOOL WINAPI HookCloseHandle(
					HANDLE hObject   // �I�u�W�F�N�g�̃n���h��
				)
{
	// �W������/�o��/�G���[�̃n���h���ł͂Ȃ�����
	if( GetStdHandle(STD_INPUT_HANDLE)  != hObject &&
		GetStdHandle(STD_OUTPUT_HANDLE) != hObject &&
		GetStdHandle(STD_ERROR_HANDLE)  != hObject )
	{
		// �Ǘ��ς݂̏��ɑ��݂��邱��
		std::map<HANDLE, FILE_INFO>::iterator
			ite = g_mapFileInfo.find(hObject);
		if( g_mapFileInfo.end() != ite )
		{
			if( !ite->second.data.empty() )
			{
				BOOL (WINAPI *pfnOrig)(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED) = 
					  (BOOL (WINAPI *)(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED))
						GetProcAddress( GetModuleHandleA(_T("kernel32.dll")), _T("WriteFile") );
				DWORD dwWrite, dwLen = ite->second.data.size();
				pfnOrig(GetStdHandle(STD_ERROR_HANDLE), ite->second.filename.c_str(), ite->second.filename.size() + 1, &dwWrite, NULL);
				pfnOrig(GetStdHandle(STD_ERROR_HANDLE), _T("\r\n"), 2, &dwWrite, NULL);
				pfnOrig(GetStdHandle(STD_ERROR_HANDLE), &dwLen, sizeof(dwLen), &dwWrite, NULL);
				pfnOrig(GetStdHandle(STD_ERROR_HANDLE), &ite->second.data[0], ite->second.data.size(), &dwWrite, NULL);
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
	if( GetStdHandle(STD_INPUT_HANDLE)  != hFile &&
		GetStdHandle(STD_OUTPUT_HANDLE) != hFile &&
		GetStdHandle(STD_ERROR_HANDLE)  != hFile )
	{
		// �Ǘ��ς݂̏��ɑ��݂��邱��
		std::map<HANDLE, FILE_INFO>::iterator
			ite = g_mapFileInfo.find(hFile);
		if( g_mapFileInfo.end() != ite )
		{
			// �������ݓ��e���t�@�C���ɏ������Ƀ������ɕێ�
			ite->second.data.resize(ite->second.offset + nNumberOfBytesToWrite);
			if( !ite->second.data.empty() ) {
				::memcpy(&ite->second.data[0] + ite->second.offset, lpBuffer, nNumberOfBytesToWrite);
			}
			ite->second.offset += nNumberOfBytesToWrite;

			return TRUE;
		}
	}

	// �I���W�i����API���Ăяo��
	BOOL r = WriteFile_(hFile,lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten,lpOverlapped);

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

void InstallAPIHook()
{
	// �I���W�i����API��ۑ�
	if( !LoadLibraryA_ ) {
		LoadLibraryA_	= LoadLibraryA;
		CreateFileA_	= CreateFileA;
		CloseHandle_	= CloseHandle;
		WriteFile_		= WriteFile;
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
}

void UninstallAPIHook()
{
	UNINSTALL_HOOK(_T("kernel32.dll"), _T("LoadLibraryA"),	HookLoadLibraryA,	NULL);
	UNINSTALL_HOOK(_T("kernel32.dll"), _T("CreateFileA"),	HookCreateFileA,	NULL);
	UNINSTALL_HOOK(_T("kernel32.dll"), _T("CloseHandle"),	HookCloseHandle,	NULL);
	UNINSTALL_HOOK(_T("kernel32.dll"), _T("WriteFile"),		HookWriteFile,		NULL);
}

