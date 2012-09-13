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
	// �^���̗̈�Ċ��蓖�Ď��ɐ擪�A�h���X�ƌ����o���Ă���
	if( size == size / sizeof(HSP3TYPEINFO) * sizeof(HSP3TYPEINFO) ) {
		type_info_top = (HSP3TYPEINFO*)r;
		type_info_num = size / sizeof(HSP3TYPEINFO);
	}
	return r;
}

int cmdfunc(int cmd)
{
	//		���s���� (���ߎ��s���ɌĂ΂�܂�)
	//
	code_next(); // ���̃R�[�h���擾(�ŏ��ɕK���K�v�ł�)

	switch(cmd)
	{
	case 0:
		if( type_info_top ) {
			// ���x�Ȃǂɉe�����Ȃ��悤���������蓖�Ċ֐��̃t�b�N������
			sbExpandHook.uninjection_code(type_info_top->hspexinfo->HspFunc_expand, sbExpandOld);
			// cmdfunc�Ȃǂ��t�b�N
			HSP3TYPEINFO* type_infolast = type_info_top + type_info_num;
			if( g_app ) {
				g_app->hook(type_info_top, type_infolast);
			}
			// �O�̂��ߍē˓����Ă����Ȃ��悤�ɖڈ�
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
	//		�v���O�C�������� (���s�E�I��������o�^���܂�)
	//

	// ���������蓖�Ċ֐����t�b�N
	sbExpandOld = (char*(*)(char*,int))sbExpandHook.injection_code(info->hspexinfo->HspFunc_expand);

	// �Ƃ肠�����̌^���̐擪�ƌ����擾
	type_info_top = info - info->type;
	type_info_num = TYPE_USERDEF
	              + type_info_top->hspctx->hsphed->max_hpi / sizeof(HPIDAT)
	              + type_info_top->hspctx->hsphed->max_varhpi;

	// cmdfunc�t�b�N�����̎���
	info->cmdfunc  = cmdfunc;
	info->termfunc = termfunc;
}
