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

HSP3DEBUG *g_hsp3dbg = NULL;

//--------------------------------------------------------------------
// デバッグ処理デバッグウインドウ側関数
//--------------------------------------------------------------------

EXPORT BOOL WINAPI debugini(HSP3DEBUG *p1, int p2, int p3, int p4)
{
	//		debugini ptr  (type1)

	g_hsp3dbg = p1;
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

//--------------------------------------------------------------------
// デバッグ処理プラグイン側関数
//--------------------------------------------------------------------

BYTE* old_code_next = NULL;

__declspec(naked)
void hook_code_next()
{
	g_hsp3dbg->dbg_curinf();

	char tmp[256];
	sprintf(tmp,"hook L:%d",g_hsp3dbg->line);
	g_app->putLog(tmp, strlen(tmp));

	// 元コード呼び出し
	_asm { jmp old_code_next }
}

EXPORT void WINAPI hspdbg_init(HSP3TYPEINFO * info)
{
	//		プラグイン初期化 (実行・終了処理を登録します)
	//
	//hsp3sdk_init( info );			// SDKの初期化(最初に行なって下さい)
	//info->cmdfunc = cmdfunc;		// 実行関数(cmdfunc)の登録
	//info->reffunc = reffunc;		// 参照関数(reffunc)の登録
	//info->termfunc = termfunc;		// 終了関数(termfunc)の登録

	BYTE* code_next = (BYTE*)info->hspexinfo->HspFunc_prm_next;

	DWORD op;
	// 関数フックのため一旦属性を書き込みできるようにする
	VirtualProtect(code_next, 5, PAGE_EXECUTE_READWRITE, &op);

	// 次コード読み込み関数処理前に関数を割り込ます
	old_code_next = (BYTE*)((DWORD)code_next + *((DWORD*)(code_next+1)) + 5); // ※jmp命令があることを前提にしている...
	*          code_next     = 0xE9; // jmp
	*((DWORD*)(code_next+1)) = (DWORD)hook_code_next - (DWORD)code_next - 5;

	// 改変検知のためページ属性を元に戻す
	VirtualProtect(code_next, 5, op, &op);

	/*
	//	イベントコールバックを発生させるイベント種別を設定する
    info->option = HSPEVENT_ENABLE_GETKEY;
	info->eventfunc = eventfunc;	// イベント関数(eventfunc)の登録
	*/
}
