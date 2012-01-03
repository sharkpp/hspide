#include "stdafx.h"
#include <windows.h>
#include <map>
#include <vector>
#include <string>
#include "apihook.h"

void InstallAPIHookCore(HMODULE handle);

typedef struct {
	std::string			filename;
	DWORD				offset;
	std::vector<BYTE>	data;
} FILE_INFO;

std::map<HANDLE, FILE_INFO> g_mapFileInfo;

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
	HANDLE (WINAPI *pfnOrig)(LPCTSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE) = 
	      (HANDLE (WINAPI *)(LPCTSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE))
			GetProcAddress( GetModuleHandleA(_T("kernel32.dll")), _T("CreateFileA") );

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
	HANDLE r = pfnOrig(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,
	                   dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);

//TCHAR tmp[256];
//sprintf(tmp,"%s(%s) = %p\n", __FUNCTION__, lpFileName, r);
//OutputDebugString(tmp);

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
	BOOL (WINAPI *pfnOrig)(HANDLE) = 
	      (BOOL (WINAPI *)(HANDLE))
			GetProcAddress( GetModuleHandleA(_T("kernel32.dll")), _T("CloseHandle") );

//TCHAR tmp[256];
//sprintf(tmp,"%s(%p)\n", __FUNCTION__, hObject);
//OutputDebugString(tmp);

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
	BOOL r = pfnOrig(hObject);

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
	BOOL (WINAPI *pfnOrig)(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED) = 
	      (BOOL (WINAPI *)(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED))
			GetProcAddress( GetModuleHandleA(_T("kernel32.dll")), _T("WriteFile") );

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

//TCHAR tmp[256];
//sprintf(tmp,"%s(%p,%d)\n", __FUNCTION__, hFile, nNumberOfBytesToWrite);
//OutputDebugString(tmp);

			return TRUE;
		}
	}

	// �I���W�i����API���Ăяo��
	BOOL r = pfnOrig(hFile,lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten,lpOverlapped);

	return r;
}

HMODULE WINAPI HookLoadLibraryA(
					LPCTSTR lpFileName   // ���W���[���̃t�@�C����
				)
{
	HMODULE (WINAPI *pfnOrig)(LPCTSTR) = 
	      (HMODULE (WINAPI *)(LPCTSTR))
			GetProcAddress( GetModuleHandleA(_T("kernel32.dll")), _T("LoadLibraryA") );
	// �I���W�i����API���Ăяo��
	HMODULE r = pfnOrig(lpFileName);
//TCHAR tmp[256];
//sprintf(tmp,"%s(%s) = %p\n", __FUNCTION__, lpFileName, r);
//OutputDebugString(tmp);

	// �ǂݍ���DLL�Ŏg�p���Ă���API�Ƀt�b�N���d�|����
	InstallAPIHookCore(r);

	return r;
}

void InstallAPIHook()
{
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
