// hspcmp.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	printf("[%s]\n", GetCommandLine());
	Sleep(10*1000);
	return 0;
}

