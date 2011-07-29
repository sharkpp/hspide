// hspcmp.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "hspcmp.h"

#if defined(_UNICODE) || defined(UNICODE)
#error "Building in unicode is not supported"
#endif

typedef enum {
	PACK_OPT_NORMAL = 0,
	PACK_OPT_ENCRYPT,
} PACK_OPTION;

typedef std::vector<std::pair<PACK_OPTION, std::string> > PACK_LIST_TYPE;

struct option {
	const char * exename;
	const char * filename;
	PACK_LIST_TYPE	packlist;
	const char * packfile;
	bool		update_packlist;
	int			wx, wy;
	bool		hidden;
//	bool		compile_only;
	option()
		: exename(0), filename(0), packfile(0)
		, update_packlist(false), wx(-1), wy(-1), hidden(false)
	{}
#ifdef _DEBUG
	void dump() {
		printf("option.exename = \"%s\"\n", exename);
		printf("option.filename = \"%s\"\n", filename);
		for(size_t i = 0; i < packlist.size(); i++) {
			printf("option.packlist[%d] = %s\"%s\"\n", i, packlist[i].first, packlist[i].second);
		}
		printf("option.packfile = \"%s\"\n", packfile);
		printf("option.update_packlist = %s\n", update_packlist ? "true" : "false");
		printf("option.wx = %d\n", wx);
		printf("option.wy = %d\n", wy);
		printf("option.hidden = %s\n", hidden ? "true" : "false");
//		printf("option.compile_only = %s\n", compile_only ? "true" : "false");
	}
#endif
} option;

void usage()
{
	static const char * help = 
		"hspcmp [Hot Soup Processer cui compiler]\n"
		"usage: hspcmp.exe option filename [packname ...]\n"
		"  option:\n"
		"    -n NAME , --name NAME\n"
		"    -s X,Y , --size X,Y\n"
		"    -I , --hidden\n"
		"    -p PACKFILE , --packfile PACKFILE\n"
		"    -d , --debug\n"
//		"    -c , --compile-only\n"
//		"    -c , --compile-only\n"
		"   packname is normal packing\n"
		"  +packname is encryption packing\n"
		;
	puts(help);
	exit(-1);
}

bool read_args(int argc, char * argv[])
{
	for(int i = 1; i < argc; i++)
	{
		if( '-' == *argv[i] ) {
			// オプション指定
			       if( (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--name")) && i + 1 < argc ) {
				// 実行ファイル名の指定
				option.filename = argv[++i];
			} else if( (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--size")) && i + 1 < argc ) {
				// ウインドウサイズの指定
				i++;
				char * ep;
				int wx = strtol(argv[i], &ep, 10);
				if( ep && (',' == *ep || 'x' == *ep) ) {
					int wy = strtol(argv[i], &ep, 10);
					option.wx = wx;
					option.wy = wy;
				}
			} else if(  !strcmp(argv[i], "-I") || !strcmp(argv[i], "--hidden") ) {
				// ウインドウの非表示の指定
				option.hidden = true;
			} else if( (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--packfile")) && i + 1 < argc ) {
				// 既存のパックファイルの指定
				option.packfile = argv[i];
			} else {
				return false;
			}
		} else {
			char * filename = argv[i];
			if( !option.filename ) { // コンパイル対象を指定
				option.filename = filename;
			} else {
				// パック対象の一覧に追加
				option.update_packlist = true;
				if( '+' == *filename ) { // 暗号化して追加
					filename++;
					option.packlist.push_back(std::make_pair(PACK_OPT_ENCRYPT, filename));
				} else {
					option.packlist.push_back(std::make_pair(PACK_OPT_NORMAL, filename));
				}
			}
		}
	}
#ifdef _DEBUG
	option.dump();
#endif
	return true;
}

bool read_pack_file(const char * packfile, PACK_LIST_TYPE & packlist)
{
	std::ifstream ifs(packfile);
	std::string buff;
	while( std::getline(ifs, buff) && !buff.empty() )
	{
		if( '+' == buff[0] ) {
			option.packlist.push_back(std::make_pair(PACK_OPT_ENCRYPT, buff));
		} else {
			option.packlist.push_back(std::make_pair(PACK_OPT_NORMAL, buff));
		}
	}
	return true;
}

bool write_pack_file(const char * packfile, const PACK_LIST_TYPE & packlist)
{
	std::ofstream ofs(packfile);
	for(PACK_LIST_TYPE::const_iterator
			ite = packlist.begin(),
			last= packlist.end();
		ite != last; ++ite)
	{
		ofs << (PACK_OPT_ENCRYPT == ite->first ? "+" : "") << ite->second << std::endl;
	}
	return true;
}

int main(int argc, char * argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0); // 出力バッファリング無効

	if( !read_args(argc, argv) ||
		!option.filename )
	{
		return -1;
	}

	char tmp[256*1024] = "";
	int len = 0;
	hspcmp cmp;

//	cmp.hsc_ver(tmp);
//	OutputDebugString(tmp);

	// コンパイル対象を指定
	cmp.hsc_ini(option.filename);

	//
	cmp.hsc_comp(false, false, false);
	//cmp.hsc3_messize(&len);
	//cmp.hsc_getmes(tmp);
	//OutputDebugString(tmp);

//	printf("[%s]\n", GetCommandLine());
//	for(int i = 0; i < 100; i++) {
//		Sleep(100);
//		printf(">>%d\n", i);
//	}

	return 0;
}

