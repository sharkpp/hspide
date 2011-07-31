// hspcmp.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <stdarg.h>
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
	bool		version;
	bool		help;
	const char * exename;
	const char * filename;
	PACK_LIST_TYPE	packlist;
	const char * packfile;
	bool		update_packlist;
	int			wx, wy;
	bool		hidden_window;
	bool		debug;
	bool		preprocess_only;
	bool		auto_make;
	bool		execute;
	const char * refname;
	const char * objname;
	const char * common_path;
	option()
		: version(false), help(false)
		, exename(NULL), filename(NULL), packfile(NULL)
		, update_packlist(false), wx(-1), wy(-1), hidden_window(false)
		, debug(false), preprocess_only(false), auto_make(false), execute(false)
		, refname(NULL), objname(NULL), common_path(NULL)
	{}
#ifdef _DEBUG
	void dump() {
		printf("------------------------------------\n");
		printf("option.version = %s\n", version ? "true" : "false");
		printf("option.help = %s\n", help ? "true" : "false");
		printf("option.exename = \"%s\"\n", exename);
		printf("option.filename = \"%s\"\n", filename);
		for(size_t i = 0; i < packlist.size(); i++) {
			printf("option.packlist[%d] = %s\"%s\"\n", i, packlist[i].first, packlist[i].second);
		}
		printf("option.packfile = \"%s\"\n", packfile);
		printf("option.update_packlist = %s\n", update_packlist ? "true" : "false");
		printf("option.wx = %d\n", wx);
		printf("option.wy = %d\n", wy);
		printf("option.hidden_window = %s\n", hidden_window ? "true" : "false");
		printf("option.debug = %s\n", debug ? "true" : "false");
		printf("option.preprocess_only = %s\n", preprocess_only ? "true" : "false");
		printf("option.auto_make = %s\n", auto_make ? "true" : "false");
		printf("option.refname = \"%s\"\n", refname);
		printf("option.objname = \"%s\"\n", objname);
		printf("option.execute = %s\n", execute ? "true" : "false");
		printf("option.common_path = \"%s\"\n", common_path);
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
		"    -a , --auto_make\n"
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
			} else if( (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--packfile")) && i + 1 < argc ) {
				// �����̃p�b�N�t�@�C���̎w��
				option.packfile = argv[i];
			} else if(  !strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug") ) {
				// �f�o�b�O
				option.debug = true;
			} else if(  !strcmp(argv[i], "-P") || !strcmp(argv[i], "--preprocess-only") ) {
				// �v���v���Z�X�����̂�
				option.preprocess_only = true;
			} else if(  !strcmp(argv[i], "-a") || !strcmp(argv[i], "--auto-make") ) {
				// ��������
				option.auto_make = true;
			} else if( (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--refname")) && i + 1 < argc ) {
				// �Q�ƃt�@�C�����̎w��
				option.refname = argv[i];
			} else if( (!strcmp(argv[i], "-C") || !strcmp(argv[i], "--common-path")) && i + 1 < argc ) {
				// ���ʃf�B���N�g���̎w��
				option.common_path = argv[i];
			} else if(  !strcmp(argv[i], "-e") || !strcmp(argv[i], "--execute") ) {
				// ���s
				option.execute = true;
			} else if(  !strcmp(argv[i], "-v") || !strcmp(argv[i], "--version") ) {
				// �o�[�W�����̕\��
				option.version = true;
			} else if(  !strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || !strcmp(argv[i], "--?") ) {
				// �w���v�̕\��
				option.help = true;
			} else {
				return false;
			}
		} else {
			char * filename = argv[i];
			if( !option.filename ) { // �R���p�C���Ώۂ��w��
				option.filename = filename;
			} else {
				// �p�b�N�Ώۂ̈ꗗ�ɒǉ�
				option.update_packlist = true;
				if( '+' == *filename ) { // �Í������Ēǉ�
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
	hspcmp cmp;

	if( option.version ) {
		tmp.resize(1024*1024);
		cmp.hsc_ver(&tmp[0]);
		print_message("hspcmp.dll : %s\n", &tmp[0]);
		return 0;
	}
//	OutputDebugString(tmp);

	// ���ʃf�B���N�g�����w��
	if( option.common_path ) {
		std::string common_path = option.common_path;
		normalize_directory(common_path);
		if( (result = cmp.hsc_compath(common_path.c_str())) < 0 ) {
			print_message("can't set common path!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
			return -1;
		}
printf("common_path='%s'\n",common_path.c_str());
	}
	// �R���p�C���Ώۂ��w��
	if( (result = cmp.hsc_ini(option.filename)) < 0 ) {
		print_message("can't set target name!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
		return -1;
	}

	// �Q�ƃt�@�C�������w��
	if( option.refname ) {
		if( (result = cmp.hsc_refname(option.refname)) < 0 ) {
			print_message("can't set reference name!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
			return -1;
		}
	}

	// �I�u�W�F�N�g�����w��
	if( option.objname ) {
		if( (result = cmp.hsc_objname(option.objname)) < 0 ) {
			print_message("can't set object name!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
			return -1;
		}
	} else {
		get_filename(option.filename, objname, false);
		objname += ".ax";
		option.objname = objname.c_str();
	}
option.dump();

	// �R���p�C��
	result = cmp.hsc_comp(
					 (option.debug?1:0)				// �f�o�b�O or �����[�X
					|(option.preprocess_only?2:0)	// �v���v���Z�X�����̂�
					,(option.auto_make?4:0)			// ����EXE����
					,(option.hidden_window?0:1)		// �E�C���h�E���\��
				);
	// �G���[���b�Z�[�W�擾���\��
	print_compiler_message(cmp);
	// �G���[�̏ꍇ�͏I��
	if( result < 0 ) {
		return -1;
	}

	// ���s
	if( option.execute )
	{
		std::vector<char> runtime(MAX_PATH*2+1);
		std::string cmdline;
		if( (result = cmp.hsc3_getruntime(&runtime[0], option.objname)) < 0 ) {
			print_message("can't get runtime name!\n"); // ����A��ɐ��폈���ɂȂ�̂ŏ�����ʂ�Ȃ�
			return -1;
		}
		// �f�t�H���g�̃����^�C�������w��
		if( !runtime[0] ) {
			static const char DEFAULT_RUNTIME[] = "hsp3.exe";
			const static size_t SIZEOF_DEFAULT_RUNTIME = sizeof(DEFAULT_RUNTIME);
			runtime.assign(DEFAULT_RUNTIME, DEFAULT_RUNTIME + SIZEOF_DEFAULT_RUNTIME);
		}
		cmdline = std::string(&runtime[0]) + " \"" + std::string(option.objname) + "\"";
printf("'%s'\n", cmdline.c_str());
		if( (result = cmp.hsc3_run(cmdline.c_str())) < 0 ) {
			// �G���[���b�Z�[�W�擾���\��
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

