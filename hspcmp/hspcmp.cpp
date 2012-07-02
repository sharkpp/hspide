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

static
struct option {
	bool		version;			// バージョン表示のみ
	bool		help;				// ヘルプの表示のみ
	const char * exename;			// 実行ファイル名
	const char * filename;			// 処理ファイル名
	packfile_manager
				packfile;			// パックファイル処理
	int			exetype;			// 実行ファイル種別
	int			wx, wy;				// ウインドウサイズ(wx=幅,wy=高さ)
	bool		hidden_window;		// 生成した実行ファイルで起動時にウインドウを非表示にしておくか
	bool		debug;				// デバッグモード
	bool		debug_window;		// デバッグウインドウ
	bool		preprocess_only;	// プリプロセス処理のみ行う
	bool		auto_make;			// 自動EXE生成を行う
	bool		make;				// EXE生成処理を行う
	bool		execute;			// コンパイルが正常終了した場合、実行
	bool		symbol_put;			// 定義済みシンボルの表示
	bool		redirect;			// オブジェクトファイルなどをファイルに書き込まず標準エラー出力にリダイレクト
	bool		hspide_fix;			// hspide用に色々修正
	const char * refname;			// コンパイル結果などに表示するファイル名
	const char * objname;			// オブジェクトファイル名
	const char * common_path;		// commonフォルダのパス
	const char * hsp_path;			// HSPフォルダのパス
	const char * work_path;			// 作業ディレクトリのパス
	const char * cmdline;			// EXE実行時にコマンドラインへ渡す内容
	const char * attach;			// デバッガとの関連付けの情報
	option()
		: version(false), help(false)
		, exename(NULL), filename(NULL)
		, exetype(EXE_TYPE_DEFAULT), wx(0), wy(0), hidden_window(false)
		, debug(false), debug_window(false), preprocess_only(false)
		, auto_make(false), make(false), execute(false)
		, symbol_put(false)
		, redirect(false)
		, hspide_fix(false)
		, refname(NULL), objname(NULL), common_path(NULL), hsp_path(NULL), work_path(NULL)
		, cmdline(NULL)
		, attach(NULL)
	{}
#ifdef _DEBUG
	void dump() {
		printf("!!! ------------------------------------\n");
		printf("!!! option.version = %s\n", version ? "true" : "false");
		printf("!!! option.help = %s\n", help ? "true" : "false");
		printf("!!! option.exename = \"%s\"\n", exename);
		printf("!!! option.filename = \"%s\"\n", filename);
		printf("!!! option.exetype = %d\n", exetype);
		printf("!!! option.wx = %d\n", wx);
		printf("!!! option.wy = %d\n", wy);
		printf("!!! option.hidden_window = %s\n", hidden_window ? "true" : "false");
		printf("!!! option.debug = %s\n", debug ? "true" : "false");
		printf("!!! option.debug_window = %s\n", debug_window ? "true" : "false");
		printf("!!! option.preprocess_only = %s\n", preprocess_only ? "true" : "false");
		printf("!!! option.auto_make = %s\n", auto_make ? "true" : "false");
		printf("!!! option.make = %s\n", make ? "true" : "false");
		printf("!!! option.refname = \"%s\"\n", refname);
		printf("!!! option.objname = \"%s\"\n", objname);
		printf("!!! option.execute = %s\n", execute ? "true" : "false");
		printf("!!! option.common_path = \"%s\"\n", common_path);
		printf("!!! option.hsp_path = \"%s\"\n", hsp_path);
		printf("!!! option.work_path = \"%s\"\n", work_path);
		printf("!!! option.cmdline = \"%s\"\n", cmdline);
		printf("!!! option.symbol_put = %s\n", symbol_put ? "true" : "false");
		printf("!!! option.redirect = %s\n", redirect ? "true" : "false");
		printf("!!! option.hspide_fix = %s\n", hspide_fix ? "true" : "false");
		printf("!!! option.attach = \"%s\"\n", attach);
		packfile.dump();
	}
#endif
} option;

void InstallAPIHook(bool redirect, bool hspide_fix);
void UninstallAPIHook();

void usage()
{
	hspcmp cmp;
	char version[256];
	if( cmp.loaded() ) {
		cmp.hsc_ver(version);
	} else {
		strcpy(version, "not found");
	}
	static const char * help = 
		"hspcmp [Hot Soup Processer command line compiler]\n"
		"  Copyright(c) 2011-2012 by sharkpp All rights reserved.\n"
		"  [ hspcmp.dll(%s) ]\n"
		"\n"
		"usage: hspcmp.exe option filename [packname ...]\n"
		"  option:\n"
		"    -h , --help , -?\n"
		"    -v , --version\n"
		"    -n NAME , --name NAME\n"
		"    -T TYPE , --type TYPE\n"
		"        0 or \"exe\" -> default\n"
		"        2 or \"scr\" -> screen saver\n"
		"    -s X,Y , --size X,Y\n"
		"    -I , --hidden\n"
		"    -d , --debug\n"
		"    -D , --debug-window\n"
		"    -P , --preprocess-only\n"
		"    -a , --auto-make\n"
		"        auto making exe file, ignore packname\n"
		"    -m , --make\n"
		"    -r REFERENCE_NAME , --refname REFERENCE_NAME\n"
		"    -o OBJCT_NAME , --objname OBJCT_NAME\n"
		"    -e , --execute\n"
		"    -C COMMON_PATH , --common-path COMMON_PATH\n"
		"    -H HSP_PATH , --hsp-path HSP_PATH\n"
		"    -w WORK_PATH , --work-path WORK_PATH\n"
		"    -c COMMAND_LINE , --cmdline COMMAND_LINE\n"
		"    --symbol-put\n"
		"   packname:\n"
		"     packname : normal packing\n"
		"    +packname : encryption packing\n"
		;
	printf(help, version);
	exit(-1);
}

bool read_args(int argc, char * argv[])
{
	bool debug_window_set = false; // デバッグウインドウ表示が指示されているか？

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
			} else if( (!strcmp(argv[i], "-T") || !strcmp(argv[i], "--type")) && i + 1 < argc ) {
				// 実行ファイル種別
				i++;
				       if( !strcmp(argv[i], "exe") ) {
					option.exetype = EXE_TYPE_DEFAULT;
				} else if( !strcmp(argv[i], "scr") ) {
					option.exetype = EXE_TYPE_SCREENSAVER;
				} else {
					char * ep;
					int exetype = strtol(argv[i], &ep, 10);
					if( ep ) {
						option.exetype = exetype;
					}
				}
			} else if(  !strcmp(argv[i], "-I") || !strcmp(argv[i], "--hidden") ) {
				// ウインドウの非表示の指定
				option.hidden_window = true;
			} else if(  !strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug") ) {
				// デバッグ
				option.debug = true;
				if( !debug_window_set ) {
					option.debug_window = true;
				}
			} else if(  !strcmp(argv[i], "-D") || !strcmp(argv[i], "--debug-window") ) {
				// デバッグウインドウ
				option.debug_window = true;
				debug_window_set = true;
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
				option.refname = argv[++i];
			} else if( (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--objname")) && i + 1 < argc ) {
				// オブジェクトファイル名の指定
				option.objname = argv[++i];
			} else if( (!strcmp(argv[i], "-C") || !strcmp(argv[i], "--common-path")) && i + 1 < argc ) {
				// 共通ディレクトリの指定
				option.common_path = argv[++i];
			} else if( (!strcmp(argv[i], "-H") || !strcmp(argv[i], "--hsp-path")) && i + 1 < argc ) {
				// HSPディレクトリの指定
				option.hsp_path = argv[++i];
			} else if( (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--work-path")) && i + 1 < argc ) {
				// 作業ディレクトリの指定
				option.work_path = argv[++i];
			} else if(  !strcmp(argv[i], "-e") || !strcmp(argv[i], "--execute") ) {
				// 実行
				option.execute = true;
			} else if(  !strcmp(argv[i], "-c") || !strcmp(argv[i], "--cmdline") ) {
				// 実行時の引数指定
				option.cmdline = argv[++i];
			} else if(  !strcmp(argv[i], "--attach") ) {
				// デバッガ用
				option.attach = argv[++i];
			} else if(  !strcmp(argv[i], "--redirect") ) {
				// オブジェクトファイルなどをファイルに書き込まず標準エラー出力にリダイレクト
				option.redirect = true;
			} else if(  !strcmp(argv[i], "--hspide-fix") ) {
				// hspide用に色々修正
				option.hspide_fix = true;
			} else if(  !strcmp(argv[i], "-v") || !strcmp(argv[i], "--version") ) {
				// バージョンの表示
				option.version = true;
			} else if(  !strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || !strcmp(argv[i], "--?") ) {
				// ヘルプの表示
				option.help = true;
			} else if(  !strcmp(argv[i], "--symbol-put") ) {
				// シンボル一覧の表示
				option.symbol_put = true;
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

// ファイル名を取得
void basename(const char * path, std::string & filename, bool with_extension)
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

// フォルダ名を取得
void dirname(const char * path, std::string & filename)
{
	const char * last_sep = path;
	for(const char * p = path; *p; p = (const char*)CharNext(p)) {
		if( '\\' == *p ) {
			last_sep = p;
		}
	}
	filename.assign(last_sep, size_t(last_sep - path));
	if( filename.empty() ) {
		filename = ".";
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

	// 引数解析
	if( !read_args(argc, argv) ||
		((!option.filename || option.help) && !option.version && !option.symbol_put) )
	{
#ifdef _DEBUG
		option.dump();
#endif
		usage();
		return -1;
	}

	InstallAPIHook(option.redirect, option.hspide_fix);

	hspcmp cmp;
	std::vector<char> tmp;
	int result;
	std::string common_path;
	std::string hsp_path;
	std::string work_path;
	std::string objname;
	std::string runtime;
	std::string exename;
	std::string tmp2;
	
	if( option.attach ) {
		// デバッガとの関連付け用の情報を渡す
		static const char KEY[] = "hspide#attach=";
		static const size_t KEY_LEN = sizeof(KEY) - 1;
		tmp.assign(KEY, KEY + KEY_LEN);
		tmp.insert(tmp.end(), option.attach, option.attach + strlen(option.attach));
		_putenv(&tmp[0]);
	}

	if( option.version ) {
		// バージョンのみを表示
		tmp.resize(1024*1024);
		cmp.hsc_ver(&tmp[0]);
		print_message("hspcmp.dll : %s\n", &tmp[0]);
		UninstallAPIHook();
		return 0;
	}

	if( option.symbol_put ) {
		// 定義済みシンボルを表示
		tmp.resize(1024*1024);
		cmp.hsc3_getsym();
		print_compiler_message(cmp);
		UninstallAPIHook();
		return 0;
	}

	if( !option.work_path ) {
		work_path = ".\\";
		option.work_path = work_path.c_str();
	}

	// 共通ディレクトリを指定
	if( !option.common_path && option.hsp_path ) {
		common_path = option.hsp_path;
		normalize_directory(common_path);
		common_path+= "common";
		option.common_path = common_path.c_str();
	}
	if( option.common_path ) {
		std::string common_path = option.common_path;
		normalize_directory(common_path);
		if( (result = cmp.hsc_compath(common_path.c_str())) != 0 ) {
			print_message("can't set common path!\n"); // 現状、常に正常処理になるので処理を通らない
			UninstallAPIHook();
			return -1;
		}
#ifdef _DEBUG
		printf("common_path='%s'\n",common_path.c_str());
#endif
	}

	if( !option.hsp_path ) {
		// HSPのディレクトリが指定されていなかったらカレントとする
		hsp_path = ".";
		option.hsp_path = hsp_path.c_str();
	}

	// コンパイル対象を指定
	if( (result = cmp.hsc_ini(option.filename)) != 0 ) {
		print_message("can't set target name!\n"); // 現状、常に正常処理になるので処理を通らない
		UninstallAPIHook();
		return -1;
	}

	// 参照ファイル名を指定
	if( option.refname ) {
		if( (result = cmp.hsc_refname(option.refname)) != 0 ) {
			print_message("can't set reference name!\n"); // 現状、常に正常処理になるので処理を通らない
			UninstallAPIHook();
			return -1;
		}
	}

	// オブジェクト名を指定
	if( option.objname || option.make || option.auto_make ) {
		if( option.make || option.auto_make ) {
			objname = "start.ax";
			option.objname = objname.c_str();
		}
		if( (result = cmp.hsc_objname(option.objname)) != 0 ) {
			print_message("can't set object name!\n"); // 現状、常に正常処理になるので処理を通らない
			UninstallAPIHook();
			return -1;
		}
	} else {
		basename(option.filename, objname, false);
		objname += ".ax";
		option.objname = objname.c_str();
	}
#if !defined(NDEBUG)
	option.dump();
#endif

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
	if( result != 0 ) {
		UninstallAPIHook();
		return -1;
	}

	// ランタイムを取得
	tmp.resize(MAX_PATH*2+1);
	if( (result = cmp.hsc3_getruntime(&tmp[0], option.objname)) != 0 ) {
		print_message("can't get runtime name!\n"); // 現状、常に正常処理になるので処理を通らない
		UninstallAPIHook();
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
	print_message("#runtime '%s'\n", runtime.c_str());
	print_message("#objname '%s'\n", option.objname);

	if( option.make || option.auto_make || option.execute )
	{
		// 実行ファイル名を指定
		if( !option.exename ) {
			basename(option.filename, exename, false);
		} else {
			basename(option.exename, exename, false);
		}
		option.exename = exename.c_str();
	}

	if( option.make )
	{
		// パックファイル生成
		option.packfile.save(option.work_path);

		if( (result = cmp.pack_ini(option.exename)) != 0 ) {
			print_message("can't set pack name!\n"); // 現状、常に正常処理になるので処理を通らない
			UninstallAPIHook();
			return -1;
		}

		if( (result = cmp.pack_make(CREATE_DPM_EXE, ENCRYPTE_KEY_DEFAULT)) != 0 ) {
			// エラーメッセージ取得＆表示
			print_compiler_message(cmp);
			UninstallAPIHook();
			return -1;
		}

		if( (result = cmp.pack_opt(
							  option.wx, option.wy
							, option.hidden_window ? EXE_OPT_HIDDEN_WINDOW : 0
						)) != 0 )
		{
			print_message("can't set pack option!\n"); // 現状、常に正常処理になるので処理を通らない
			UninstallAPIHook();
			return -1;
		}

		// ランタイム指定
		tmp2  = option.hsp_path;
		normalize_directory(tmp2);
		tmp2 += runtime;
		if( (result = cmp.pack_rt(tmp2.c_str())) != 0 ) {
			print_message("can't set runtime name!\n"); // 現状、常に正常処理になるので処理を通らない
			UninstallAPIHook();
			return -1;
		}

		// 実行ファイル生成
		if( EXE_TYPE_FULLSCREEN == option.exetype ) {
			print_message("fullscreen option is now obsolete.\n");
		}
		if( option.exetype < 0 || EXE_TYPE_SCREENSAVER < option.exetype ) {
			option.exetype = EXE_TYPE_DEFAULT;
			print_message("exe type is ignored.\n");
		}
		if( (result = cmp.pack_exe(option.exetype)) != 0 ) {
			// エラーメッセージ取得＆表示
			print_compiler_message(cmp);
			UninstallAPIHook();
			return -1;
		}
	}
	else if( option.auto_make )
	{
		// 実行ファイル生成
		tmp2  = option.hsp_path;
		normalize_directory(tmp2);
		tmp2 += runtime;
		if( (result = cmp.hsc3_make(tmp2.c_str())) != 0 ) {
			// エラーメッセージ取得＆表示
			print_compiler_message(cmp);
			UninstallAPIHook();
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
			cmdline = option.hsp_path;
			normalize_directory(cmdline);
			cmdline = "\"" + cmdline + runtime + "\" \"" + std::string(option.objname) + "\"";
		}
		if( option.cmdline ) {
			cmdline += " ";
			cmdline += option.cmdline;
		}
#if !defined(NDEBUG)
		printf("CMDLINE='%s'\n", cmdline.c_str());
#endif
		if( (result = cmp.hsc3_run(cmdline.c_str())) != 0 ) {
			// エラーメッセージ取得＆表示
			print_compiler_message(cmp);
			print_message("can't execute!\n");
			UninstallAPIHook();
			return -1;
		}
	}

	UninstallAPIHook();
	return 0;
}

