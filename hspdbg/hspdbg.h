
#include "hspsdk/hsp3debug.h"

#ifdef HSPDBG_EXPORTS
#define EXPORT extern "C" __declspec(dllexport)
#endif

EXPORT BOOL WINAPI debugini(HSP3DEBUG *p1, int p2, int p3, int p4);
EXPORT BOOL WINAPI debug_notice(HSP3DEBUG *p1, int p2, int p3, int p4);
