#include "stdafx.h"
#include <windows.h>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <spplib/apihook.h>

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
	bool				userdef_as;
} FILE_INFO;

//-------------------------------------------------------------------
// 変数定義

bool g_bRedirectMode = false;
bool g_bHSPideFix = false;

// ファイル情報
std::map<HANDLE, FILE_INFO> g_mapFileInfo;

// 標準ハンドルチェック用情報
std::set<HANDLE> g_setStdXXX;

// フックを行うAPIのオリジナルを保存しておく変数
static HANDLE  (WINAPI *  CreateFileA_)(LPCTSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE) = NULL;
static BOOL    (WINAPI *  CloseHandle_)(HANDLE) = NULL;
static BOOL    (WINAPI *    WriteFile_)(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED) = NULL;
static BOOL    (WINAPI *    ReadFile_)(HANDLE,LPVOID,DWORD,LPDWORD,LPOVERLAPPED) = NULL;
static HMODULE (WINAPI * LoadLibraryA_)(LPCTSTR) = NULL;
static DWORD   (WINAPI * SetFilePointer_)(HANDLE,LONG,PLONG,DWORD) = NULL;

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
	bool redirect_mine = false;
	bool userdef_as	= g_bHSPideFix && !lstrcmpi(lpFileName, "userdef.as");

	if( g_bRedirectMode )
	{
		if( dwDesiredAccess & GENERIC_WRITE )
		{
			hook_filename = lpFileName;
			// ダミー情報に書き換え
			::GetModuleFileName(NULL, filename, MAX_PATH*2);
			lpFileName				= filename;
			dwDesiredAccess			= 0;
			dwCreationDisposition	= OPEN_EXISTING;
		}
	}

	// オリジナルのAPIを呼び出し
	HANDLE r = CreateFileA_(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,
	                        dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);

	if( 0 != (dwDesiredAccess & GENERIC_READ) &&
		userdef_as )
	{
		hook_filename = lpFileName;
		// もう一度開きなおす
		if( INVALID_HANDLE_VALUE == r )
		{
			// ダミー情報に書き換え
			::GetModuleFileName(NULL, filename, MAX_PATH*2);
			lpFileName				= filename;
			dwDesiredAccess			= 0;
			dwCreationDisposition	= OPEN_EXISTING;
			redirect_mine			= true;
			// もう一度呼び出し
			r = CreateFileA_(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,
							 dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
		}
	}

	if( INVALID_HANDLE_VALUE != r &&
		!hook_filename.empty() )
	{
		// I/O処理をフックするための情報を登録
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
					HANDLE hObject   // オブジェクトのハンドル
				)
{
	// 標準入力/出力/エラーのハンドルではないこと
	if( g_setStdXXX.end() == g_setStdXXX.find(hObject) )
	{
		// 管理済みの情報に存在すること
		std::map<HANDLE, FILE_INFO>::iterator
			ite = g_mapFileInfo.find(hObject);
		if( g_mapFileInfo.end() != ite )
		{
			FILE_INFO& fi = ite->second;
			// ファイル名とサイズと内容を書き出す
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
	if( g_setStdXXX.end() == g_setStdXXX.find(hFile) )
	{
		// 管理済みの情報に存在すること
		std::map<HANDLE, FILE_INFO>::iterator
			ite = g_mapFileInfo.find(hFile);
		if( g_mapFileInfo.end() != ite )
		{
			FILE_INFO& fi = ite->second;
			// 書き込み内容をファイルに書かずにメモリに保持
			fi.data.resize(fi.offset + nNumberOfBytesToWrite);
			if( !fi.data.empty() ) {
				::memcpy(&fi.data[0] + fi.offset, lpBuffer, nNumberOfBytesToWrite);
			}
			fi.offset += nNumberOfBytesToWrite;
			return TRUE;
		}
	}

	// オリジナルのAPIを呼び出し
	BOOL r = WriteFile_(hFile,lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten,lpOverlapped);

	return r;
}

BOOL WINAPI HookReadFile(
					HANDLE hFile,                // ファイルのハンドル
					LPVOID lpBuffer,             // データバッファ
					DWORD nNumberOfBytesToRead,  // 読み取り対象のバイト数
					LPDWORD lpNumberOfBytesRead, // 読み取ったバイト数
					LPOVERLAPPED lpOverlapped    // オーバーラップ構造体のバッファ
				)
{
	// 標準入力/出力/エラーのハンドルではないこと
	if( g_setStdXXX.end() == g_setStdXXX.find(hFile) )
	{
		// 管理済みの情報に存在すること
		std::map<HANDLE, FILE_INFO>::iterator
			ite = g_mapFileInfo.find(hFile);
		if( g_mapFileInfo.end() != ite )
		{
			FILE_INFO& fi = ite->second;
			// 別の内容を挿入した内容を書き出す
			if( fi.userdef_as )
			{
				DWORD read = (std::min)(nNumberOfBytesToRead, fi.data.size() - fi.offset);
				::memcpy(lpBuffer, &fi.data[0], read);
				*lpNumberOfBytesRead = read;
				return TRUE;
			}
		}
	}

	// オリジナルのAPIを呼び出し
	BOOL r = ReadFile_(hFile,lpBuffer,nNumberOfBytesToRead,lpNumberOfBytesRead,lpOverlapped);

	// 管理されている対象か？
	if( FALSE != r && g_bHSPideFix )
	{
		// #regcmd 18 を探して書き換え
		static const char   SEARCH_STR_REGCMD[] = "#regcmd ";
		static const size_t SEARCH_LEN_REGCMD = sizeof(SEARCH_STR_REGCMD) - 1; // '\0'の分だけ減らす
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
					HANDLE hFile,                // ファイルのハンドル
					LONG lDistanceToMove,        // ポインタを移動するべきバイト数
					PLONG lpDistanceToMoveHigh,  // ポインタを移動するべきバイト数
					DWORD dwMoveMethod           // 開始点
				)
{
	// 標準入力/出力/エラーのハンドルではないこと
	if( g_setStdXXX.end() == g_setStdXXX.find(hFile) )
	{
		// 管理済みの情報に存在すること
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

	// オリジナルのAPIを呼び出し
	DWORD r = SetFilePointer_(hFile,lDistanceToMove,lpDistanceToMoveHigh,dwMoveMethod);

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

void InstallAPIHook(bool redirect, bool hspide_fix)
{
	g_bRedirectMode = redirect;
	g_bHSPideFix    = hspide_fix;

	if( !g_bRedirectMode && !g_bHSPideFix ) {
		return;
	}

	// オリジナルのAPIを保存
	if( !LoadLibraryA_ )
	{
		LoadLibraryA_	= LoadLibraryA;
		CreateFileA_	= CreateFileA;
		CloseHandle_	= CloseHandle;
		WriteFile_		= WriteFile;
		ReadFile_		= ReadFile;
		SetFilePointer_	= SetFilePointer;

		// 標準入出力ハンドルを比較のために保存
		g_setStdXXX.insert(GetStdHandle(STD_INPUT_HANDLE));
		g_setStdXXX.insert(GetStdHandle(STD_OUTPUT_HANDLE));
		g_setStdXXX.insert(GetStdHandle(STD_ERROR_HANDLE));
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

