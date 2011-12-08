// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "stdafx.h"
#include <stdlib.h>
#include <QCoreApplication>
#include <QLocalSocket>

static int argc_ = 1;
static char *argv_[] = {""};
QCoreApplication* g_app = NULL;
QLocalSocket*	g_dbgSocket = NULL;
long long		g_dbgId = -1;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
MessageBox(NULL,TEXT(""),TEXT(""),0);
		g_app = new QCoreApplication(argc_, argv_);
		g_dbgId = _strtoui64(getenv("hspide#attach"), NULL, 16);
		g_dbgSocket = new QLocalSocket(g_app);
		g_dbgSocket->connectToServer("test@hspide");
		break;
	case DLL_PROCESS_DETACH:
		g_dbgSocket->disconnectFromServer();
		delete g_app;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

