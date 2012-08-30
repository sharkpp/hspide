// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "stdafx.h"
#include <stdlib.h>
#include "dbgmain.h"
#include "hsp3debug.h"

CDbgMain*  g_app = NULL;
HANDLE     g_appHandle = NULL;
hsp3debug* g_hsp3debug_dll = NULL;

BOOL APIENTRY DllMain(HMODULE /*hModule*/,
                      DWORD  ul_reason_for_call,
                      LPVOID /*lpReserved*/
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
		// オリジナルのDLL呼び出しクラスを生成
		if( !getenv("hspide") ) {
			g_hsp3debug_dll = new hsp3debug("hsp3debug.old.dll");
			if( !g_hsp3debug_dll->loaded() ) {
				return FALSE;
			}
		}
		break;
	case DLL_PROCESS_DETACH:
		CDbgMain::destroy();
		// オリジナルのDLL呼び出しクラスを破棄
		if( g_hsp3debug_dll ) {
			delete g_hsp3debug_dll;
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

