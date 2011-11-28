// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "stdafx.h"
#include <stdlib.h>
#include "ipc_qt_client_win.hpp"

QtLocalSocket	g_dbgSocket;
long long		g_dbgId = -1;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_dbgId = _strtoui64(getenv("hspide#attach"), NULL, 16);
		g_dbgSocket.connectToServer("test@hspide");
		break;
	case DLL_PROCESS_DETACH:
		g_dbgSocket.disconnectFromServer();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

