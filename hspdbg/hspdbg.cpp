// hspdbg.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include <stdlib.h>
#include "hspdbg.h"
#include "ipc_qt_client_win.hpp"
#include "hspsdk/hsp3struct.h"

extern QtLocalSocket	g_dbgSocket;
extern long long		g_dbgId;

EXPORT BOOL WINAPI debugini(HSP3DEBUG *p1, int p2, int p3, int p4)
{
	//		debugini ptr  (type1)

	BYTE   data[256];
	size_t len = 0;
	long long id = _strtoui64(getenv("hspide#attach"), NULL, 16);
	memcpy(data + len, &id, sizeof(id)); len += sizeof(id);
	data[len] = 0x00; len++;
	g_dbgSocket.writeData((char*)data, len);

//	QtLocalSocket sock;
//	sock.writeData(__FUNCTION__, strlen(__FUNCTION__)+1);
//	sock.writeData("/", 1);
//	sock.writeData(getenv("hspide#attach"), strlen(getenv("hspide#attach")));
//	sock.writeData("\n", 1);
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
		long long id = _strtoui64(getenv("hspide#attach"), NULL, 16);
		HSPCTX* ctx = (HSPCTX*)p1->hspctx;
		memcpy(data + len, &id, sizeof(id)); len += sizeof(id);
		data[len] = 0x01; len++;
		int size = strlen(ctx->stmp);
		memcpy(data + len, &size, sizeof(size)); len += sizeof(size);
		memcpy(data + len, ctx->stmp, size); len += size;
		g_dbgSocket.writeData((char*)data, len);
	}

//	QtLocalSocket sock;
//	g_sock.connectToServer("test@hspide");
//	g_sock.writeData(__FUNCTION__, strlen(__FUNCTION__)+1);
//	g_sock.writeData("/", 1);
//	g_sock.writeData(getenv("hspide#attach"), strlen(getenv("hspide#attach")));
//	g_sock.writeData("\n", 1);
	return FALSE;
}

