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

	if( !g_app )
	{
		// メイン処理ルーティンを生成
		CDbgMain::create();
	}

	g_app->initialize(p1);

	g_app->updateInfo();

	g_hsp3dbg = p1;

	return FALSE;
}

EXPORT BOOL WINAPI debug_notice(HSP3DEBUG *p1, int p2, int p3, int p4)
{
	//		debug_notice ptr  (type1)
	//			p2: 0=stop event
	//			    1=send message

	HSPCTX* hspctx = (HSPCTX*)p1->hspctx;

	if( 1 == p2 )
	{
		g_app->putLog(hspctx->stmp, strlen(hspctx->stmp));
	}

	p1->dbg_curinf();

int line = p1->line;
int runmode = hspctx->runmode;

	bool breaked = g_app->isBreak(p1->fname, p1->line);

	if( breaked )
	{
		g_app->updateInfo();

		hspctx->runmode = RUNMODE_STOP;
	}

		hspctx->runmode = RUNMODE_RUN;
	int r =
	p1->dbg_set( breaked ? HSPDEBUG_STOP : HSPDEBUG_STEPIN );

char tmp[256];
sprintf(tmp,"%s(%2d) p2=%d runmode=%d,breaked=%d,r=%d",p1->fname?p1->fname:"???",line,p2,runmode,breaked,r);
g_app->putLog(tmp, strlen(tmp));

	return FALSE;
}

//--------------------------------------------------------------------
// デバッグ処理プラグイン側関数
//--------------------------------------------------------------------

EXPORT void WINAPI hspdbg_init(HSP3TYPEINFO * info)
{
	//		プラグイン初期化 (実行・終了処理を登録します)
	//
	HSP3TYPEINFO* top = info - info->type;
	HSP3TYPEINFO* last= top  + info->type + 1;
	HSP3TYPEINFO* ite = top;

	if( !g_app )
	{
		// メイン処理ルーティンを生成
		CDbgMain::create();
	}

	g_app->hook(top, last);
	//for(; ite < last; ++ite)
	//{
	//	// 書き換え
	//	g_cmdfunc[ite - top].install_hook(&ite->cmdfunc);
	//}
}
