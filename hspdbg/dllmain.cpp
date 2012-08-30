// dllmain.cpp : DLL �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
#include "stdafx.h"
#include <stdlib.h>
#include <tchar.h>
#include "dbgmain.h"
#include "hsp3debug.h"

CDbgMain*  g_app = NULL;
HANDLE     g_appHandle = NULL;
hsp3debug* g_hsp3debug_dll = NULL;

BOOL APIENTRY DllMain(HMODULE /*hModule*/,
                      DWORD  ul_reason_for_call,
                      LPVOID /*lpReserved*/
                      )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
#ifdef _DEBUG
		// �V�t�g�L�[�����Ă���ƃA�^�b�`�����邽�߂Ƀ_�C�A���O���o��
		if( GetAsyncKeyState(VK_SHIFT) < 0 ) {
			MessageBox(NULL,TEXT("Attach me"),TEXT("HSP3"),MB_OK|MB_SYSTEMMODAL);
		}
#endif
		// �I���W�i����DLL�Ăяo���N���X�𐶐�
#if __STDC_WANT_SECURE_LIB__
		size_t envSize;
		_tgetenv_s(&envSize, NULL, 0, "hspide");
		if( !envSize )
#else
		if( !getenv("hspide") )
#endif
		{
			g_hsp3debug_dll = new hsp3debug("hsp3debug.old.dll");
			if( !g_hsp3debug_dll->loaded() ) {
				return FALSE;
			}
		}
		break; }
	case DLL_PROCESS_DETACH:
		CDbgMain::destroy();
		// �I���W�i����DLL�Ăяo���N���X��j��
		if( g_hsp3debug_dll ) {
			delete g_hsp3debug_dll;
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

