// hspdbg.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include <stdlib.h>
#include <QLocalSocket>
#include <QByteArray>
#include "dbgmain.h"
#include "hspdbg.h"
#include "hspsdk/hsp3struct.h"
#include "hsp3debug.h"

extern CDbgMain* g_app;
extern hsp3debug* g_hsp3debug_dll;

HSP3DEBUG *g_hsp3dbg = NULL;

//--------------------------------------------------------------------
// デバッグ処理デバッグウインドウ側関数
//--------------------------------------------------------------------

EXPORT BOOL WINAPI debugini(HSP3DEBUG *p1, int /*p2*/, int /*p3*/, int /*p4*/)
{
	//		debugini ptr  (type1)

	// オリジナルを呼び出し
	if( g_hsp3debug_dll )
	{
		return g_hsp3debug_dll->debugini(p1);
	}

	if( !g_app )
	{
		// メイン処理ルーティンを生成
		CDbgMain::create();
	}

	g_app->initialize(p1);

	g_hsp3dbg = p1;

	g_app->updateInfo();

	return FALSE;
}

EXPORT BOOL WINAPI debug_notice(HSP3DEBUG *p1, int p2, int /*p3*/, int /*p4*/)
{
	//		debug_notice ptr  (type1)
	//			p2: 0=stop event
	//			    1=send message

	// オリジナルを呼び出し
	if( g_hsp3debug_dll )
	{
		return g_hsp3debug_dll->debug_notice(p1, p2);
	}

	HSPCTX* hspctx = (HSPCTX*)p1->hspctx;

	if( 1 == p2 )
	{
		g_app->putLog(hspctx->stmp, strlen(hspctx->stmp));
	}
	else
	{
		g_app->updateInfo();
	}

	return FALSE;
}

//--------------------------------------------------------------------
// デバッグ処理プラグイン側関数
//--------------------------------------------------------------------

namespace {
HSP3TYPEINFO* top;
}

#define code_next top->hspctx->exinfo.HspFunc_prm_next
#define puterror  top->hspctx->exinfo2->HspFunc_puterror

int cmdfunc(int cmd)
{
	//		実行処理 (命令実行時に呼ばれます)
	//
	code_next(); // 次のコードを取得(最初に必ず必要です)

	switch(cmd)
	{
	case 0:
		if( top ) {
			HSP3TYPEINFO* last = top + TYPE_USERDEF + top->hspctx->hsphed->max_hpi / sizeof(HPIDAT) + top->hspctx->hsphed->max_varhpi;
			g_app->hook(top, last);
			top = NULL;
			break;
		}
	default:
		puterror(HSPERR_UNSUPPORTED_FUNCTION);
	}
	return RUNMODE_RUN;
}

int termfunc(int /*option*/)
{
OutputDebugStringA("CDbgMain::termfunc #1\n");
	g_app->disconnectFromDebugger();
OutputDebugStringA("CDbgMain::termfunc #2\n");
	return 0;
}

EXPORT void WINAPI hspdbg_init(HSP3TYPEINFO * info)
{
	//		プラグイン初期化 (実行・終了処理を登録します)
	//

	top  = info - info->type;

	info->cmdfunc  = cmdfunc;
	info->termfunc = termfunc;
}
