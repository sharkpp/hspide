
#include "module_loader.hpp"

class hspcmp
	: public spplib::module_loader<hspcmp>
{
public:

	hspcmp()
		: module_loader(_T("hspcmp"))
	{
		def("_hsc_ini@16",         _hsc_ini        );
		def("_hsc_refname@16",     _hsc_refname    );
		def("_hsc_objname@16",     _hsc_objname    );
		def("_hsc_compath@16",     _hsc_compath    );
		def("_hsc_comp@16",        _hsc_comp       );
		def("_hsc_getmes@16",      _hsc_getmes     );
		def("_hsc_clrmes@16",      _hsc_clrmes     );
		def("_hsc_ver@16",         _hsc_ver        );
		def("_hsc_bye@16",         _hsc_bye        );
		def("_hsc3_getsym@16",     _hsc3_getsym    );
		def("_hsc3_messize@16",    _hsc3_messize   );
		def("_hsc3_make@16",       _hsc3_make      );
		def("_hsc3_getruntime@16", _hsc3_getruntime);
		def("_hsc3_run@16",        _hsc3_run       );
		def("_pack_ini@16",        _pack_ini       );
		def("_pack_view@16",       _pack_view      );
		def("_pack_make@16",       _pack_make      );
		def("_pack_exe@16",        _pack_exe       );
		def("_pack_opt@16",        _pack_opt       );
		def("_pack_rt@16",         _pack_rt        );
		def("_pack_get@16",        _pack_get       );
	}

	virtual ~hspcmp()
	{
		_hsc_bye(0, 0, 0, 0);
	}

	int hsc_ini        (const char * filename)                     { return _hsc_ini        (0, int(filename), 0, 0); }
	int hsc_refname    (const char * filename)                     { return _hsc_refname    (0, int(filename), 0, 0); }
	int hsc_objname    (const char * filename)                     { return _hsc_objname    (0, int(filename), 0, 0); }
	int hsc_compath    (const char * pathname)                     { return _hsc_compath    (0, int(pathname), 0, 0); }
	int hsc_comp       (bool debug, bool macro, bool debug_window) { return _hsc_comp       (debug, macro, debug_window, 0); }
	int hsc_getmes     (char * messages)                           { return _hsc_getmes     (int(messages), 0, 0, 0); }
	int hsc_clrmes     ()                                          { return _hsc_clrmes     (0, 0, 0, 0); }
	int hsc_ver        (char * version)                            { return _hsc_ver        (0, 0, 0, int(version)); }
//	int hsc_bye        (int, int, int, int) { return _hsc_bye        (0, 0, 0, 0); }
	int hsc3_getsym    ()                                          { return _hsc3_getsym    (0, 0, 0, 0); }
	int hsc3_messize   (int * size)                                { return _hsc3_messize   (int(size), 0, 0, 0); }
	int hsc3_make      (const char * path)                         { return _hsc3_make      (0, int(path), 0, 0); }
	int hsc3_getruntime(char * runtime, char * objfile)            { return _hsc3_getruntime(int(runtime), int(objfile), 0, 0); }
	int hsc3_run       (const char * cmdline)                      { return _hsc3_run       (0, int(cmdline), 0, 0); }
	int pack_ini       (const char * filename)                     { return _pack_ini       (0, int(filename), 0, 0); }
	int pack_view      ()                                          { return _pack_view      (0, 0, 0, 0); }
	int pack_make      (int type, int crypt_key)                   { return _pack_make      (type, crypt_key, 0, 0); }
	int pack_exe       (int type)                                  { return _pack_exe       (type, 0, 0, 0); }
	int pack_opt       (int wx, int wy, int sw)                    { return _pack_opt       (wx, wy, sw, 0); }
	int pack_rt        (const char * runtime)                      { return _pack_rt        (0, int(runtime), 0, 0); }
	int pack_get       (const char * filename)                     { return _pack_get       (0, int(filename), 0, 0); }

private:

	int (WINAPI *_hsc_ini        )(int, int, int, int);
	int (WINAPI *_hsc_refname    )(int, int, int, int);
	int (WINAPI *_hsc_objname    )(int, int, int, int);
	int (WINAPI *_hsc_compath    )(int, int, int, int);
	int (WINAPI *_hsc_comp       )(int, int, int, int);
	int (WINAPI *_hsc_getmes     )(int, int, int, int);
	int (WINAPI *_hsc_clrmes     )(int, int, int, int);
	int (WINAPI *_hsc_ver        )(int, int, int, int);
	int (WINAPI *_hsc_bye        )(int, int, int, int);
	int (WINAPI *_hsc3_getsym    )(int, int, int, int);
	int (WINAPI *_hsc3_messize   )(int, int, int, int);
	int (WINAPI *_hsc3_make      )(int, int, int, int);
	int (WINAPI *_hsc3_getruntime)(int, int, int, int);
	int (WINAPI *_hsc3_run       )(int, int, int, int);
	int (WINAPI *_pack_ini       )(int, int, int, int);
	int (WINAPI *_pack_view      )(int, int, int, int);
	int (WINAPI *_pack_make      )(int, int, int, int);
	int (WINAPI *_pack_exe       )(int, int, int, int);
	int (WINAPI *_pack_opt       )(int, int, int, int);
	int (WINAPI *_pack_rt        )(int, int, int, int);
	int (WINAPI *_pack_get       )(int, int, int, int);
};
