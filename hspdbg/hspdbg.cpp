// hspdbg.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include <stdlib.h>
#include <QLocalSocket>
#include <QByteArray>
#include "dbgmain.h"
#include "hspdbg.h"
#include "hspsdk/hsp3struct.h"

extern CDbgMain* g_app;

EXPORT BOOL WINAPI debugini(HSP3DEBUG *p1, int p2, int p3, int p4)
{
	//		debugini ptr  (type1)

	g_app->connectToDebugger();
	return FALSE;
}

EXPORT BOOL WINAPI debug_notice(HSP3DEBUG *p1, int p2, int p3, int p4)
{
	//		debug_notice ptr  (type1)
	//			p2: 0=stop event
	//			    1=send message

	BYTE   data[1024];
	size_t len = 0;

	if( 1 == p2 )
	{
		HSPCTX* ctx = (HSPCTX*)p1->hspctx;
		g_app->putLog(ctx->stmp, strlen(ctx->stmp));
	}

	p1->dbg_curinf();

	char tmp[256];
	sprintf(tmp,"L:%d",p1->line);
	g_app->putLog(tmp, strlen(tmp));

//	QtLocalSocket sock;
//	g_sock.connectToServer("test@hspide");
//	g_sock.writeData(__FUNCTION__, strlen(__FUNCTION__)+1);
//	g_sock.writeData("/", 1);
//	g_sock.writeData(getenv("hspide#attach"), strlen(getenv("hspide#attach")));
//	g_sock.writeData("\n", 1);

	return FALSE;
}

