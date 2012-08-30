// hspdbg.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
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
// �f�o�b�O�����f�o�b�O�E�C���h�E���֐�
//--------------------------------------------------------------------

EXPORT BOOL WINAPI debugini(HSP3DEBUG *p1, int /*p2*/, int /*p3*/, int /*p4*/)
{
	//		debugini ptr  (type1)

	// �I���W�i�����Ăяo��
	if( g_hsp3debug_dll )
	{
		return g_hsp3debug_dll->debugini(p1);
	}

	if( !g_app )
	{
		// ���C���������[�e�B���𐶐�
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

	// �I���W�i�����Ăяo��
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
// �f�o�b�O�����v���O�C�����֐�
//--------------------------------------------------------------------

namespace {
HSP3TYPEINFO* top;
}

#define code_next top->hspctx->exinfo.HspFunc_prm_next
#define puterror  top->hspctx->exinfo2->HspFunc_puterror

int cmdfunc(int cmd)
{
	//		���s���� (���ߎ��s���ɌĂ΂�܂�)
	//
	code_next(); // ���̃R�[�h���擾(�ŏ��ɕK���K�v�ł�)

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
	//		�v���O�C�������� (���s�E�I��������o�^���܂�)
	//

	top  = info - info->type;

	info->cmdfunc  = cmdfunc;
	info->termfunc = termfunc;
}
