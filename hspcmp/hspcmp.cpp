// hspcmp.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <stdarg.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "hspcmp.h"
#include "packfile_manager.hpp"

#if defined(_UNICODE) || defined(UNICODE)
#error "Building in unicode is not supported"
#endif

struct option {
	bool		version;
	bool		help;
	const char * exename;
	const char * filename;
	packfile_manager
				packfile;
	int			wx, wy;
	bool		hidden_window;
	bool		debug;
	bool		debug_window;
	bool		debug_window_set;
	bool		preprocess_only;
	bool		auto_make;
	bool		make;
	bool		execute;
	const char * refname;
	const char * objname;
	const char * common_path;
	option()
		: version(false), help(false)
		, exename(NULL), filename(NULL)
		, wx(-1), wy(-1), hidden_window(false)
		, debug(false), debug_window(false), debug_window_set(false), preprocess_only(false)
		, auto_make(false), make(false), execute(false)
		, refname(NULL), objname(NULL), common_path(NULL)
	{}
#ifdef _DEBUG
	void dump() {
		printf("------------------------------------\n");
		printf("option.version = %s\n", version ? "true" : "false");
		printf("option.help = %s\n", help ? "true" : "false");
		printf("option.exename = \"%s\"\n", exename);
		printf("option.filename = \"%s\"\n", filename);
		printf("option.wx = %d\n", wx);
		printf("option.wy = %d\n", wy);
		printf("option.hidden_window = %s\n", hidden_window ? "true" : "false");
		printf("option.debug = %s\n", debug ? "true" : "false");
		printf("option.debug_window = %s\n", debug_window ? "true" : "false");
		printf("option.debug_window_set = %s\n", debug_window_set ? "true" : "false");
		printf("option.preprocess_only = %s\n", preprocess_only ? "true" : "false");
		printf("option.auto_make = %s\n", auto_make ? "true" : "false");
		printf("option.make = %s\n", make ? "true" : "false");
		printf("option.refname = \"%s\"\n", refname);
		printf("option.objname = \"%s\"\n", objname);
		printf("option.execute = %s\n", execute ? "true" : "false");
		printf("option.common_path = \"%s\"\n", common_path);
		packfile.dump();
	}
#endif
} option;

void usage()
{
	static const char * help = 
		"hspcmp [Hot Soup Processer cui compiler]\n"
		"usage: hspcmp.exe option filename [packname ...]\n"
		"  option:\n"
		"    -h , --help , -?\n"
		"    -v , --version\n"
		"    -n NAME , --name NAME\n"
		"    -s X,Y , --size X,Y\n"
		"    -I , --hidden\n"
		"    -p PACKFILE , --packfile PACKFILE\n"
		"    -d , --debug\n"
		"    -P , --preprocess-only\n"
		"    -a , --auto-make\n"
		"    -m , --make\n"
		"    -r REFERENCE_NAME , --refname REFERENCE_NAME\n"
		"    -o OBJCT_NAME , --objname OBJCT_NAME\n"
		"    -e , --execute\n"
		"    -C COMMON_PATH , --common-path COMMON_PATH\n"
		"   packname:\n"
		"     packname : normal packing\n"
		"    +packname : encryption packing\n"
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
				option.hidden_window = true;
			} else if( (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--packfile")) && i + 1 < argc ) {
				// 既存のパックファイルの指定
				option.packfile.load(argv[i]);
			} else if(  !strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug") ) {
				// デバッグ
				option.debug = true;
				if( !option.debug_window_set ) {
					option.debug_window = true;
				}
			} else if(  !strcmp(argv[i], "-D") || !strcmp(argv[i], "--debug-window") ) {
				// デバッグウインドウ
				option.debug_window = true;
				option.debug_window_set = true;
			} else if(  !strcmp(argv[i], "-P") || !strcmp(argv[i], "--preprocess-only") ) {
				// プリプロセス処理のみ
				option.preprocess_only = true;
			} else if(  !strcmp(argv[i], "-a") || !strcmp(argv[i], "--auto-make") ) {
				// 自動生成
				option.auto_make = true;
			} else if(  !strcmp(argv[i], "-m") || !strcmp(argv[i], "--make") ) {
				// 生成
				option.make = true;
			} else if( (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--refname")) && i + 1 < argc ) {
				// 参照ファイル名の指定
				option.refname = argv[i];
			} else if( (!strcmp(argv[i], "-C") || !strcmp(argv[i], "--common-path")) && i + 1 < argc ) {
				// 共通ディレクトリの指定
				option.common_path = argv[i];
			} else if(  !strcmp(argv[i], "-e") || !strcmp(argv[i], "--execute") ) {
				// 実行
				option.execute = true;
			} else if(  !strcmp(argv[i], "-v") || !strcmp(argv[i], "--version") ) {
				// バージョンの表示
				option.version = true;
			} else if(  !strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || !strcmp(argv[i], "--?") ) {
				// ヘルプの表示
				option.help = true;
			} else {
				return false;
			}
		} else {
			char * filename = argv[i];
			if( !option.filename ) { // コンパイル対象を指定
				option.filename = filename;
				option.packfile.append("start.ax", packfile_manager::PACK_OPTION_ENCRYPT);
			} else {
				// パック対象の一覧に追加
				option.packfile.append(
						  '+' == *filename ? filename + 1 : filename
						, '+' == *filename ? packfile_manager::PACK_OPTION_ENCRYPT
						                   : packfile_manager::PACK_OPTION_NORMAL
					);
			}
		}
	}
#ifdef _DEBUG
	option.dump();
#endif
	return true;
}

void get_filename(const char * path, std::string & filename, bool with_extension)
{
	const char * filename_pos = path;
	for(; *path; path = (const char*)CharNext(path)) {
		if( '\\' == *path ) {
			filename_pos = (const char*)CharNext(path);
		}
	}
	if( with_extension ) {
		filename = filename_pos;
	} else {
		const char * dot_pos = NULL;
		for(path = filename_pos; *path;
			path = (const char*)CharNext(path))
		{
			if( '.' == *path ) {
				dot_pos = path;
			}
		}
		if( !dot_pos ) {
			dot_pos = path;
		}
		filename.assign(filename_pos, 0, size_t(dot_pos - filename_pos));
	}
}

// パスの最後にディレクトリ区切りを追加する
void normalize_directory(std::string & path)
{
	bool separator = false;
	for(const char * p = path.c_str();
		*p; p = (const char*)CharNext(p))
	{
		separator = '\\' == *p;
	}
	if( !separator ) {
		path += "\\";
	}
}

void print_message(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

bool print_compiler_message(hspcmp & cmp)
{
	int len = 0;
	cmp.hsc3_messize(&len);

	std::vector<char> tmp(len + 1);
	cmp.hsc_getmes(&tmp[0]);
	if( tmp[0] ) {
		print_message(&tmp[0]);
	}

	cmp.hsc_clrmes();
	
	return 0 != tmp[0];
}

int main(int argc, char * argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0); // 出力バッファリング無効

	if( !read_args(argc, argv) ||
		((!option.filename || option.help) && !option.version) )
	{
#ifdef _DEBUG
	option.dump();
#endif
		usage();
		return -1;
	}

	std::vector<char> tmp;
	int result;
	std::string objname;
	std::string runtime;
	hspcmp cmp;

	if( option.version ) {
		tmp.resize(1024*1024);
		cmp.hsc_ver(&tmp[0]);
		print_message("hspcmp.dll : %s\n", &tmp[0]);
		return 0;
	}
//	OutputDebugString(tmp);

	// 共通ディレクトリを指定
	if( option.common_path ) {
		std::string common_path = option.common_path;
		normalize_directory(common_path);
		if( (result = cmp.hsc_compath(common_path.c_str())) < 0 ) {
			print_message("can't set common path!\n"); // 現状、常に正常処理になるので処理を通らない
			return -1;
		}
printf("common_path='%s'\n",common_path.c_str());
	}
	// コンパイル対象を指定
	if( (result = cmp.hsc_ini(option.filename)) < 0 ) {
		print_message("can't set target name!\n"); // 現状、常に正常処理になるので処理を通らない
		return -1;
	}

	// 参照ファイル名を指定
	if( option.refname ) {
		if( (result = cmp.hsc_refname(option.refname)) < 0 ) {
			print_message("can't set reference name!\n"); // 現状、常に正常処理になるので処理を通らない
			return -1;
		}
	}

	// オブジェクト名を指定
	if( option.objname || option.make || option.auto_make ) {
		if( option.make || option.auto_make ) {
			objname = "start.ax";
			option.objname = objname.c_str();
		}
		if( (result = cmp.hsc_objname(option.objname)) < 0 ) {
			print_message("can't set object name!\n"); // 現状、常に正常処理になるので処理を通らない
			return -1;
		}
	} else {
		get_filename(option.filename, objname, false);
		objname += ".ax";
		option.objname = objname.c_str();
	}
option.dump();

	// コンパイル
	result = cmp.hsc_comp(
					 (option.debug?DEBUG_INFO_ENABLE:0)				// デバッグ or リリース
					|(option.preprocess_only?PREPROCESS_ONLY:0)		// プリプロセス処理のみ
					,(option.auto_make?AUTO_MAKE:0)					// 自動EXE生成
					,(option.debug_window?ENABLE_DEBUG_WINDOW:0)	// デバッグウインドウを表示
				);
	// エラーメッセージ取得＆表示
	print_compiler_message(cmp);
	// エラーの場合は終了
	if( result < 0 ) {
		return -1;
	}

	// ランタイムを取得
	if( option.make || option.auto_make || option.execute )
	{
		tmp.resize(MAX_PATH*2+1);
		if( (result = cmp.hsc3_getruntime(&tmp[0], option.objname)) < 0 ) {
			print_message("can't get runtime name!\n"); // 現状、常に正常処理になるので処理を通らない
			return -1;
		}
		static const char DEFAULT_RELEASE_RUNTIME[] = "hsprt";
		static const char DEFAULT_DEBUG_RUNTIME[]   = "hsp3.exe";
		// デフォルトのランタイム名を指定
		if( !tmp[0] ) {
			runtime = option.make || option.auto_make
				? DEFAULT_RELEASE_RUNTIME : DEFAULT_DEBUG_RUNTIME;
		} else {
			runtime = &tmp[0];
		}
	}

	if( option.make )
	{
		// パックファイル生成
		if( !*option.packfile.filename() ) {
			option.packfile.load("packfile");
		}
		option.packfile.save();

		if( (result = cmp.pack_ini(option.packfile.filename())) < 0 ) {
			print_message("can't set pack name!\n"); // 現状、常に正常処理になるので処理を通らない
			return -1;
		}

		if( (result = cmp.pack_make(CREATE_DPM_EXE, ENCRYPTE_KEY_DEFAULT)) < 0 ) {
			// エラーメッセージ取得＆表示
			print_compiler_message(cmp);
			return -1;
		}

		if( (result = cmp.pack_opt(
							  option.wx, option.wy
							, option.hidden_window ? EXE_OPT_HIDDEN_WINDOW : 0
						)) < 0 )
		{
			print_message("can't set pack option!\n"); // 現状、常に正常処理になるので処理を通らない
			return -1;
		}

		if( (result = cmp.pack_rt(runtime.c_str())) < 0 ) {
			print_message("can't set runtime name!\n"); // 現状、常に正常処理になるので処理を通らない
			return -1;
		}

		if( (result = cmp.pack_exe(EXE_TYPE_DEFAULT)) < 0 ) {
			// エラーメッセージ取得＆表示
			print_compiler_message(cmp);
			return -1;
		}
	}

	// 実行
	if( option.execute )
	{
		std::string cmdline;
		if( option.make || option.auto_make ) {
			cmdline = std::string(option.exename);
		} else {
			cmdline = runtime + " \"" + std::string(option.objname) + "\"";
		}
printf("'%s'\n", cmdline.c_str());
		if( (result = cmp.hsc3_run(cmdline.c_str())) < 0 ) {
			// エラーメッセージ取得＆表示
			print_compiler_message(cmp);
			print_message("can't execute!\n");
			return -1;
		}
	}

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

