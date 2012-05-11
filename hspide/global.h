#include "configuration.h"
#include "filemanager.h"
#include "breakpointmanager.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

extern Configuration     theConf;
extern FileManager       theFile;
extern BreakPointManager theBreakPoint;
