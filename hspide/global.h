#include "configuration.h"
#include "filemanager.h"
#include "breakpointmanager.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_E2FD1FCC_5681_4EEB_AF9F_844C2BE460BB
#define INCLUDE_GUARD_E2FD1FCC_5681_4EEB_AF9F_844C2BE460BB

extern Configuration*     theConf;
extern FileManager*       theFile;
extern BreakPointManager* theBreakPoint;

// macro
#ifndef _countof
#define _countof(v) (sizeof(v)/sizeof((v)[0]))
#endif

#endif // !efined(INCLUDE_GUARD_E2FD1FCC_5681_4EEB_AF9F_844C2BE460BB)
