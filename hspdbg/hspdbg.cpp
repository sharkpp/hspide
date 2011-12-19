// hspdbg.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
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
// �f�o�b�O�����f�o�b�O�E�C���h�E���֐�
//--------------------------------------------------------------------

EXPORT BOOL WINAPI debugini(HSP3DEBUG *p1, int p2, int p3, int p4)
{
	//		debugini ptr  (type1)

	// ���C���������[�e�B���𐶐�
	CDbgMain::create();

	g_hsp3dbg = p1;

	return FALSE;
}

EXPORT BOOL WINAPI debug_notice(HSP3DEBUG *p1, int p2, int p3, int p4)
{
	//		debug_notice ptr  (type1)
	//			p2: 0=stop event
	//			    1=send message

	HSPCTX* ctx = (HSPCTX*)p1->hspctx;

	if( 1 == p2 )
	{
		g_app->putLog(ctx->stmp, strlen(ctx->stmp));
	}

	p1->dbg_curinf();

	char tmp[256];
	sprintf(tmp,"%s(%d) p2=%d runmode=%d",p1->fname?p1->fname:"???",p1->line,p2,ctx->runmode);
	g_app->putLog(tmp, strlen(tmp));

	p1->dbg_set( HSPDEBUG_STEPIN );

	return FALSE;
}

//--------------------------------------------------------------------
// �f�o�b�O�����v���O�C�����֐�
//--------------------------------------------------------------------

EXPORT void WINAPI hspdbg_init(HSP3TYPEINFO * info)
{
	//		�v���O�C�������� (���s�E�I��������o�^���܂�)
	//
}
