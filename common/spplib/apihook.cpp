#define WIN32_LEAN_AND_MEAN
#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <Dbghelp.h>

// ImageDirectoryEntryToData
#pragma comment(lib, "Dbghelp.lib")

// http://ruffnex.oc.to/kenji/text/api_hook/ex2.cpp
// ひとつのモジュールに対してAPIフックを行う関数
bool ReplaceIATEntryInOneMod(
                             PCSTR pszModuleName,
                             PROC pfnCurrent,
                             PROC pfnNew,
                             HMODULE hmodCaller) 
{
	ULONG ulSize;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
		hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

	if (pImportDesc == NULL) {
		return false;
	}

	while(pImportDesc->Name) {
		PSTR pszModName = (PSTR) ((PBYTE) hmodCaller + pImportDesc->Name);
		if (lstrcmpiA(pszModName, pszModuleName) == 0) 
			break;
		pImportDesc++;
	}

	if (pImportDesc->Name == 0) {
		return false;
	}

	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA) 
		((PBYTE) hmodCaller + pImportDesc->FirstThunk);

	while(pThunk->u1.Function) {
		PROC *ppfn = (PROC*) &pThunk->u1.Function;
		BOOL fFound = (*ppfn == pfnCurrent);
		if (fFound) {
			DWORD dwDummy;
			VirtualProtect(ppfn, sizeof(ppfn), PAGE_EXECUTE_READWRITE, &dwDummy);
			WriteProcessMemory(
				GetCurrentProcess(), ppfn, &pfnNew, sizeof(pfnNew), NULL);
			return true;
		}
		pThunk++;
	}
	return false;
}

// すべてのモジュールに対してAPIフックを行う関数
bool ReplaceIATEntryInAllMods(
                              PCSTR pszModuleName,
                              PROC pfnCurrent,
                              PROC pfnNew,
                              HMODULE hmodCaller) 
{
	if( hmodCaller ) {
		return ReplaceIATEntryInOneMod(pszModuleName, pfnCurrent, pfnNew, hmodCaller);
	} else {
		bool bResult = false;
		// モジュールリストを取得
		HANDLE hModuleSnap = CreateToolhelp32Snapshot(
								TH32CS_SNAPMODULE, GetCurrentProcessId());
		if(hModuleSnap == INVALID_HANDLE_VALUE) {
			return bResult;
		}

		MODULEENTRY32 me;
		me.dwSize = sizeof(me);
		BOOL bModuleResult = Module32First(hModuleSnap, &me);
		// それぞれのモジュールに対してReplaceIATEntryInOneModを実行
		while(bModuleResult) {
//			OutputDebugString(">>");
//			OutputDebugString(me.szModule);
//			OutputDebugString("\n");
			if( ReplaceIATEntryInOneMod(pszModuleName, pfnCurrent, pfnNew, me.hModule) ) {
				bResult = true;
			}
			bModuleResult = Module32Next(hModuleSnap, &me);
		}
		CloseHandle(hModuleSnap);
		return bResult;
	}
}
