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
// �f�o�b�O�����v���O�C�����֐�
//--------------------------------------------------------------------

BYTE* old_code_next = NULL;

__declspec(naked)
void hook_code_next()
{
	g_hsp3dbg->dbg_curinf();

	char tmp[256];
	sprintf(tmp,"hook L:%d",g_hsp3dbg->line);
	g_app->putLog(tmp, strlen(tmp));

	// ���R�[�h�Ăяo��
	_asm { jmp old_code_next }
}

EXPORT void WINAPI hspdbg_init(HSP3TYPEINFO * info)
{
	//		�v���O�C�������� (���s�E�I��������o�^���܂�)
	//
	//hsp3sdk_init( info );			// SDK�̏�����(�ŏ��ɍs�Ȃ��ĉ�����)
	//info->cmdfunc = cmdfunc;		// ���s�֐�(cmdfunc)�̓o�^
	//info->reffunc = reffunc;		// �Q�Ɗ֐�(reffunc)�̓o�^
	//info->termfunc = termfunc;		// �I���֐�(termfunc)�̓o�^

	BYTE* code_next = (BYTE*)info->hspexinfo->HspFunc_prm_next;

	DWORD op;
	// �֐��t�b�N�̂��߈�U�������������݂ł���悤�ɂ���
	VirtualProtect(code_next, 5, PAGE_EXECUTE_READWRITE, &op);

	// ���R�[�h�ǂݍ��݊֐������O�Ɋ֐������荞�܂�
	old_code_next = (BYTE*)((DWORD)code_next + *((DWORD*)(code_next+1)) + 5); // ��jmp���߂����邱�Ƃ�O��ɂ��Ă���...
	*          code_next     = 0xE9; // jmp
	*((DWORD*)(code_next+1)) = (DWORD)hook_code_next - (DWORD)code_next - 5;

	// ���ό��m�̂��߃y�[�W���������ɖ߂�
	VirtualProtect(code_next, 5, op, &op);

	/*
	//	�C�x���g�R�[���o�b�N�𔭐�������C�x���g��ʂ�ݒ肷��
    info->option = HSPEVENT_ENABLE_GETKEY;
	info->eventfunc = eventfunc;	// �C�x���g�֐�(eventfunc)�̓o�^
	*/
}
