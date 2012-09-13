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

char* sbExpand(char *ptr, int size);

namespace {

HSP3TYPEINFO* type_info_top = NULL;
size_t        type_info_num = 0;

spplib::thunk sbExpandHook(sbExpand);
char* (*sbExpandOld)(char *ptr, int size);

}

#define code_next type_info_top->hspctx->exinfo.HspFunc_prm_next
#define puterror  type_info_top->hspctx->exinfo2->HspFunc_puterror

char* sbExpand(char *ptr, int size)
{
	char* r = sbExpandOld(ptr, size);
	// 型情報の領域再割り当て時に先頭アドレスと個数を覚えておく
	if( size == size / sizeof(HSP3TYPEINFO) * sizeof(HSP3TYPEINFO) ) {
		type_info_top = (HSP3TYPEINFO*)r;
		type_info_num = size / sizeof(HSP3TYPEINFO);
	}
	return r;
}

int cmdfunc(int cmd)
{
	//		実行処理 (命令実行時に呼ばれます)
	//
	code_next(); // 次のコードを取得(最初に必ず必要です)

	switch(cmd)
	{
	case 0:
		if( type_info_top ) {
			// 速度などに影響しないようメモリ割り当て関数のフックを解除
			sbExpandHook.uninjection_code(type_info_top->hspexinfo->HspFunc_expand, sbExpandOld);
			// cmdfuncなどをフック
			HSP3TYPEINFO* type_infolast = type_info_top + type_info_num;
			if( g_app ) {
				g_app->hook(type_info_top, type_infolast);
			}
			// 念のため再突入しても問題ないように目印
			type_info_top = NULL;
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
	if( g_app )
	{
		g_app->disconnectFromDebugger();
		g_app->unhook();
	}
OutputDebugStringA("CDbgMain::termfunc #2\n");
	return 0;
}

EXPORT void WINAPI hspdbg_init(HSP3TYPEINFO * info)
{
	//		プラグイン初期化 (実行・終了処理を登録します)
	//

	// メモリ割り当て関数をフック
	sbExpandOld = (char*(*)(char*,int))sbExpandHook.injection_code(info->hspexinfo->HspFunc_expand);

	// とりあえずの型情報の先頭と個数を取得
	type_info_top = info - info->type;
	type_info_num = TYPE_USERDEF
	              + type_info_top->hspctx->hsphed->max_hpi / sizeof(HPIDAT)
	              + type_info_top->hspctx->hsphed->max_varhpi;

	// cmdfuncフック処理の実装
	info->cmdfunc  = cmdfunc;
	info->termfunc = termfunc;
}
