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

	// メイン処理ルーティンを生成
	CDbgMain::create();

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
		char* p;
		// デバッグ情報更新
		p = p1->get_value(DEBUGINFO_GENERAL);
		g_app->updateDebugInfo(p);
		p1->dbg_close(p);

		//		変数情報取得
		//		option
		//			bit0 : sort ( 受け側で処理 )
		//			bit1 : module
		//			bit2 : array
		//			bit3 : dump
		p = p1->get_varinf(NULL, 2|4);
		g_app->updateVarInfo(p);
		p1->dbg_close(p);

		hspctx->runmode = RUNMODE_STOP;
	}

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
}
