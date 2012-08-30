
#include <spplib/module_loader.hpp>

class hsp3debug
	: public spplib::module_loader<hsp3debug>
{
public:

	hsp3debug(const char * module_name = "hsp3debug")
		: module_loader(module_name)
	{
		def("_debug_notice@16", _debug_notice);
		def("_debugini@16",     _debugini    );
	}

	virtual ~hsp3debug()
	{
	}

	int debug_notice(HSP3DEBUG *ptr, int type) { return _debug_notice(int(ptr), type, 0, 0); }
	int debugini    (HSP3DEBUG *ptr)           { return _debugini    (int(ptr), 0, 0, 0); }

private:

	int (WINAPI *_debug_notice)(int, int, int, int);
	int (WINAPI *_debugini    )(int, int, int, int);
};
