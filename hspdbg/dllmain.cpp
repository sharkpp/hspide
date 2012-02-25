// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "stdafx.h"
#include <stdlib.h>
#include "dbgmain.h"

CDbgMain* g_app = NULL;
HANDLE    g_appHandle = NULL;

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
		// シフトキー押しているとアタッチをするためにダイアログを出す
		if( GetAsyncKeyState(VK_SHIFT) < 0 ) {
			MessageBox(NULL,TEXT("Attach me"),TEXT("HSP3"),MB_OK|MB_SYSTEMMODAL);
		}
#endif
		break;
	case DLL_PROCESS_DETACH:
		CDbgMain::destroy();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

