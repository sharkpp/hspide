#include "stdafx.h"
#include <windows.h>
#include <map>
#include <vector>
#include <string>
#include "apihook.h"

//-------------------------------------------------------------------
// 関数宣言

void InstallAPIHookCore(HMODULE handle);

//-------------------------------------------------------------------
// 型宣言

// ファイル情報
typedef struct {
	std::string			filename;	// ファイル名
	DWORD				offset;		// 書き込み位置
	std::vector<BYTE>	data;		// 書き込みデータ
} FILE_INFO;

//-------------------------------------------------------------------
// 変数定義

// ファイル情報
std::map<HANDLE, FILE_INFO> g_mapFileInfo;

// フックを行うAPIのオリジナルを保存しておく変数
static HANDLE  (WINAPI *  CreateFileA_)(LPCTSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE) = NULL;
static BOOL    (WINAPI *  CloseHandle_)(HANDLE) = NULL;
static BOOL    (WINAPI *    WriteFile_)(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED) = NULL;
static HMODULE (WINAPI * LoadLibraryA_)(LPCTSTR) = NULL;

//-------------------------------------------------------------------
// APIフック用関数実装

HANDLE WINAPI HookCreateFileA(
					LPCTSTR lpFileName,                         // ファイル名
					DWORD dwDesiredAccess,                      // アクセスモード
					DWORD dwShareMode,                          // 共有モード
					LPSECURITY_ATTRIBUTES lpSecurityAttributes, // セキュリティ記述子
					DWORD dwCreationDisposition,                // 作成方法
					DWORD dwFlagsAndAttributes,                 // ファイル属性
					HANDLE hTemplateFile                        // テンプレートファイルのハンドル
				)
{
	TCHAR filename[MAX_PATH*2+1];

	std::string hook_filename;
	if( dwDesiredAccess & GENERIC_WRITE )
	{
		hook_filename = lpFileName;
		// ダミー情報に書き換え
		::GetModuleFileName(NULL, filename, MAX_PATH*2);
		lpFileName				= filename;
		dwDesiredAccess			= 0;
		dwCreationDisposition	= OPEN_EXISTING;
	}

	// オリジナルのAPIを呼び出し
	HANDLE r = CreateFileA_(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,
	                        dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);

	if( !hook_filename.empty() )
	{
		// I/O処理をフックするための情報を登録
		FILE_INFO & info = g_mapFileInfo[r];
		info.filename	= hook_filename;
		info.offset		= 0;
	}

	return r;
}

BOOL WINAPI HookCloseHandle(
					HANDLE hObject   // オブジェクトのハンドル
				)
{
	// 標準入力/出力/エラーのハンドルではないこと
	if( GetStdHandle(STD_INPUT_HANDLE)  != hObject &&
		GetStdHandle(STD_OUTPUT_HANDLE) != hObject &&
		GetStdHandle(STD_ERROR_HANDLE)  != hObject )
	{
		// 管理済みの情報に存在すること
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

	// オリジナルのAPIを呼び出し
	BOOL r = CloseHandle_(hObject);

	return r;
}

BOOL WINAPI HookWriteFile(
					HANDLE hFile,                    // ファイルのハンドル
					LPCVOID lpBuffer,                // データバッファ
					DWORD nNumberOfBytesToWrite,     // 書き込み対象のバイト数
					LPDWORD lpNumberOfBytesWritten,  // 書き込んだバイト数
					LPOVERLAPPED lpOverlapped        // オーバーラップ構造体のバッファ
				)
{
	// 標準入力/出力/エラーのハンドルではないこと
	if( GetStdHandle(STD_INPUT_HANDLE)  != hFile &&
		GetStdHandle(STD_OUTPUT_HANDLE) != hFile &&
		GetStdHandle(STD_ERROR_HANDLE)  != hFile )
	{
		// 管理済みの情報に存在すること
		std::map<HANDLE, FILE_INFO>::iterator
			ite = g_mapFileInfo.find(hFile);
		if( g_mapFileInfo.end() != ite )
		{
			// 書き込み内容をファイルに書かずにメモリに保持
			ite->second.data.resize(ite->second.offset + nNumberOfBytesToWrite);
			if( !ite->second.data.empty() ) {
				::memcpy(&ite->second.data[0] + ite->second.offset, lpBuffer, nNumberOfBytesToWrite);
			}
			ite->second.offset += nNumberOfBytesToWrite;

			return TRUE;
		}
	}

	// オリジナルのAPIを呼び出し
	BOOL r = WriteFile_(hFile,lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten,lpOverlapped);

	return r;
}

HMODULE WINAPI HookLoadLibraryA(
					LPCTSTR lpFileName   // モジュールのファイル名
				)
{
	// オリジナルのAPIを呼び出し
	HMODULE r = LoadLibraryA_(lpFileName);

	// 読み込んだDLLで使用しているAPIにフックを仕掛ける
	InstallAPIHookCore(r);

	return r;
}

//-------------------------------------------------------------------
// 関数実装

void InstallAPIHook()
{
	// オリジナルのAPIを保存
	if( !LoadLibraryA_ ) {
		LoadLibraryA_	= LoadLibraryA;
		CreateFileA_	= CreateFileA;
		CloseHandle_	= CloseHandle;
		WriteFile_		= WriteFile;
	}

	// APIフック
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

