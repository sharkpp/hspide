// hspcmp.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0); // �o�̓o�b�t�@�����O����

	printf("[%s]\n", GetCommandLine());
	for(int i = 0; i < 100; i++) {
		Sleep(100);
		printf(">>%d\n", i);
	}
	return 0;
}

