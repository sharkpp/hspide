// hspdbg.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "hspdbg.h"
#include "ipc_qt_client_win.hpp"

EXPORT BOOL WINAPI debugini(HSP3DEBUG *p1, int p2, int p3, int p4)
{
	QtLocalSocket sock;
	sock.connectToServer("test@hspide");
	sock.writeData(__FUNCTION__, strlen(__FUNCTION__)+1);
	sock.writeData("\n", 1);
	return FALSE;
}

EXPORT BOOL WINAPI debug_notice(HSP3DEBUG *p1, int p2, int p3, int p4)
{
	QtLocalSocket sock;
	sock.connectToServer("test@hspide");
	sock.writeData(__FUNCTION__, strlen(__FUNCTION__)+1);
	sock.writeData("\n", 1);
	return FALSE;
}

