// hspcmp.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
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
	bool		symbol_put;
	const char * refname;
	const char * objname;
	const char * common_path;
	const char * hsp_path;
	const char * work_path;
	const char * cmdline;
	option()
		: version(false), help(false)
		, exename(NULL), filename(NULL)
		, wx(0), wy(0), hidden_window(false)
		, debug(false), debug_window(false), debug_window_set(false), preprocess_only(false)
		, auto_make(false), make(false), execute(false)
		, symbol_put(false)
		, refname(NULL), objname(NULL), common_path(NULL), hsp_path(NULL), work_path(NULL)
		, cmdline(NULL)
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
		printf("option.hsp_path = \"%s\"\n", hsp_path);
		printf("option.work_path = \"%s\"\n", work_path);
		printf("option.cmdline = \"%s\"\n", cmdline);
		printf("option.symbol_put = %s\n", symbol_put ? "true" : "false");
		packfile.dump();
	}
#endif
} option;

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
		"hspcmp [Hot Soup Processer cui compiler]\n"
		"  hspcmp.dll(%s)\n"
		"\n"
		"usage: hspcmp.exe option filename [packname ...]\n"
		"  option:\n"
		"    -h , --help , -?\n"
		"    -v , --version\n"
		"    -n NAME , --name NAME\n"
		"    -s X,Y , --size X,Y\n"
		"    -I , --hidden\n"
		"    -d , --debug\n"
		"    -P , --preprocess-only\n"
		"    -a , --auto-make\n"
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
	for(int i = 1; i < argc; i++)
	{
		if( '-' == *argv[i] ) {
			// �I�v�V�����w��
			       if( (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--name")) && i + 1 < argc ) {
				// ���s�t�@�C�����̎w��
				option.filename = argv[++i];
			} else if( (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--size")) && i + 1 < argc ) {
				// �E�C���h�E�T�C�Y�̎w��
				i++;
				char * ep;
				int wx = strtol(argv[i], &ep, 10);
				if( ep && (',' == *ep || 'x' == *ep) ) {
					int wy = strtol(argv[i], &ep, 10);
					option.wx = wx;
					option.wy = wy;
				}
			} else if(  !strcmp(argv[i], "-I") || !strcmp(argv[i], "--hidden") ) {
				// �E�C���h�E�̔�\���̎w��
				option.hidden_window = true;
			} else if(  !strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug") ) {
				// �f�o�b�O
				option.debug = true;
				if( !option.debug_window_set ) {
					option.debug_window = true;
				}
			} else if(  !strcmp(argv[i], "-D") || !strcmp(argv[i], "--debug-window") ) {
				// �f�o�b�O�E�C���h�E
				option.debug_window = true;
				option.debug_window_set = true;
			} else if(  !strcmp(argv[i], "-P") || !strcmp(argv[i], "--preprocess-only") ) {
				// �v���v���Z�X�����̂�
				option.preprocess_only = true;
			} else if(  !strcmp(argv[i], "-a") || !strcmp(argv[i], "--auto-make") ) {
				// ��������
				option.auto_make = true;
			} else if(  !strcmp(argv[i], "-m") || !strcmp(argv[i], "--make") ) {
				// ����
				option.make = true;
			} else if( (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--refname")) && i + 1 < argc ) {
				// �Q�ƃt�@�C�����̎w��
				option.refname = argv[++i];
			} else if( (!strcmp(argv[i], "-C") || !strcmp(argv[i], "--common-path")) && i + 1 < argc ) {
				// ���ʃf�B���N�g���̎w��
				option.common_path = argv[++i];
			} else if( (!strcmp(argv[i], "-H") || !strcmp(argv[i], "--hsp-path")) && i + 1 < argc ) {
				// HSP�f�B���N�g���̎w��
				option.hsp_path = argv[++i];
			} else if( (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--work-path")) && i + 1 < argc ) {
				// ��ƃf�B���N�g���̎w��
				option.work_path = argv[++i];
			} else if(  !strcmp(argv[i], "-e") || !strcmp(argv[i], "--execute") ) {
				// ���s
				option.execute = true;
			} else if(  !strcmp(argv[i], "-c") || !strcmp(argv[i], "--cmdline") ) {
				// ���s���̈����w��
				option.cmdline = argv[++i];
			} else if(  !strcmp(argv[i], "-v") || !strcmp(argv[i], "--version") ) {
				// �o�[�W�����̕\��
				option.version = true;
			} else if(  !strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || !strcmp(argv[i], "--?") ) {
				// �w���v�̕\��
				option.help = true;
			} else if(  !strcmp(argv[i], "--symbol-put") ) {
				// �V���{���ꗗ�̕\��
				option.symbol_put = true;
			} else {
				return false;
			}
		} else {
			char * filename = argv[i];
			if( !option.filename ) { // �R���p�C���Ώۂ��w��
				option.filename = filename;
				option.packfile.append("start.ax", packfile_manager::PACK_OPTION_ENCRYPT);
			} else {
				// �p�b�N�Ώۂ̈ꗗ�ɒǉ�
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

// �t�@�C�������擾
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

// �t�H���_�����擾
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

// �p�X�̍Ō�Ƀf�B���N�g����؂��ǉ�����
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
	setvbuf(stdout, NULL, _IONBF, 0); // �o�̓o�b�t�@�����O����

	if( !read_args(argc, argv) ||
		((!option.filename || option.help) && !option.version && !option.symbol_put) )
	{
#ifdef _DEBUG
		option.dump();
#endif
		usage();
		return -1;
	}

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

	if( option.version ) {
		tmp.resize(1024*1024);
		cmp.hsc_ver(&tmp[0]);
		print_message("hspcmp.dll : %s\n", &tmp[0]);
		return 0;
	}

	if( option.symbol_put ) {
		tmp.resize(1024*1024);
		cmp.hsc3_getsym();
		print_compiler_message(cmp);
		return 0;
	}

	if( !option.work_path ) {
		work_path = ".\\";
		option.work_path = work_path.c_str();
	}

	// ���ʃf�B���N�g�����w��
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
			print_message("can't set common path!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
			return -1;
		}
#ifdef _DEBUG
		printf("common_path='%s'\n",common_path.c_str());
#endif
	}

	if( !option.hsp_path ) {
		hsp_path = ".";
		option.hsp_path = hsp_path.c_str();
	}

	// �R���p�C���Ώۂ��w��
	if( (result = cmp.hsc_ini(option.filename)) != 0 ) {
		print_message("can't set target name!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
		return -1;
	}

	// �Q�ƃt�@�C�������w��
	if( option.refname ) {
		if( (result = cmp.hsc_refname(option.refname)) != 0 ) {
			print_message("can't set reference name!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
			return -1;
		}
	}

	// �I�u�W�F�N�g�����w��
	if( option.objname || option.make || option.auto_make ) {
		if( option.make || option.auto_make ) {
			objname = "start.ax";
			option.objname = objname.c_str();
		}
		if( (result = cmp.hsc_objname(option.objname)) != 0 ) {
			print_message("can't set object name!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
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

	// �R���p�C��
	result = cmp.hsc_comp(
					 (option.debug?DEBUG_INFO_ENABLE:0)				// �f�o�b�O or �����[�X
					|(option.preprocess_only?PREPROCESS_ONLY:0)		// �v���v���Z�X�����̂�
					,(option.auto_make?AUTO_MAKE:0)					// ����EXE����
					,(option.debug_window?ENABLE_DEBUG_WINDOW:0)	// �f�o�b�O�E�C���h�E��\��
				);
	// �G���[���b�Z�[�W�擾���\��
	print_compiler_message(cmp);
	// �G���[�̏ꍇ�͏I��
	if( result != 0 ) {
		return -1;
	}

	// �����^�C�����擾
	if( option.make || option.auto_make || option.execute )
	{
		tmp.resize(MAX_PATH*2+1);
		if( (result = cmp.hsc3_getruntime(&tmp[0], option.objname)) != 0 ) {
			print_message("can't get runtime name!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
			return -1;
		}
		static const char DEFAULT_RELEASE_RUNTIME[] = "hsprt";
		static const char DEFAULT_DEBUG_RUNTIME[]   = "hsp3.exe";
		// �f�t�H���g�̃����^�C�������w��
		if( !tmp[0] ) {
			runtime = option.make || option.auto_make
				? DEFAULT_RELEASE_RUNTIME : DEFAULT_DEBUG_RUNTIME;
		} else {
			runtime = &tmp[0];
		}

		// ���s�t�@�C�������w��
		if( !option.exename ) {
			basename(option.filename, exename, false);
		} else {
			basename(option.exename, exename, false);
		}
		option.exename = exename.c_str();
	}

	if( option.make || option.auto_make )
	{
		// �p�b�N�t�@�C������
		option.packfile.save(option.work_path);

		if( (result = cmp.pack_ini(option.exename)) != 0 ) {
			print_message("can't set pack name!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
			return -1;
		}

		if( (result = cmp.pack_make(CREATE_DPM_EXE, ENCRYPTE_KEY_DEFAULT)) != 0 ) {
			// �G���[���b�Z�[�W�擾���\��
			print_compiler_message(cmp);
			return -1;
		}

		if( (result = cmp.pack_opt(
							  option.wx, option.wy
							, option.hidden_window ? EXE_OPT_HIDDEN_WINDOW : 0
						)) != 0 )
		{
			print_message("can't set pack option!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
			return -1;
		}

		tmp2  = option.hsp_path;
		normalize_directory(tmp2);
		tmp2 += runtime;
		if( (result = cmp.pack_rt(tmp2.c_str())) != 0 ) {
			print_message("can't set runtime name!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
			return -1;
		}

		if( (result = cmp.pack_exe(EXE_TYPE_DEFAULT)) != 0 ) {
			// �G���[���b�Z�[�W�擾���\��
			print_compiler_message(cmp);
			return -1;
		}
	}

	// ���s
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
			// �G���[���b�Z�[�W�擾���\��
			print_compiler_message(cmp);
			print_message("can't execute!\n");
			return -1;
		}
	}

	return 0;
}

