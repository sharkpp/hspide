######################################################################
# Automatically generated by qmake (2.01a) ? 12 3 15:01:20 2011
######################################################################

TEMPLATE = vclib
TARGET = hspdbg
DEPENDPATH += . hspsdk ..\\common\\spplib
INCLUDEPATH += . ..\\common
QT += network
QT -= gui
CONFIG -= embed_manifest_dll
CONFIG += dll thread
DEFINES += HSPDBG_EXPORTS QT_NODLL
DEFINES -= QT_DLL
QMAKE_POST_LINK += copy $(TargetPath) $(ProjectDir)\\..\\$(ConfigurationName)\\hsp3debug.dll
#contains(CONFIG, static):DEFINES += QT_NODLL

# ランタイムライブラリを静的リンク 
# Qtライブラリ側も同じオプションでビルドされていること！
QMAKE_CXXFLAGS_DEBUG  -= -MDd
QMAKE_CXXFLAGS_DEBUG  += -MTd
QMAKE_CXXFLAGS_RELEASE-= -MD
QMAKE_CXXFLAGS_RELEASE+= -MT
QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:"msvcrt.lib"
QMAKE_LFLAGS_DEBUG   += /NODEFAULTLIB:"msvcrtd.lib"

# 文字コードはMBCSを使う 
#CharacterSet=2
#DEFINES -= UNICODE

# Input
HEADERS += hspdbg.h \
           stdafx.h \
           targetver.h \
           dbgmain.h \
           thunk.hpp \
           apihook.h \
           hsp3debug.h \
           hspsdk/hsp3debug.h \
           hspsdk/hsp3struct.h \
           hspsdk/hspvar_core.h \
           hspsdk/hspwnd.h \
           ../hspide/ipc_common.h \
           ../hspide/filemanager.h \
           ../hspide/breakpointmanager.h
SOURCES += dllmain.cpp hspdbg.cpp stdafx.cpp \
           dbgmain.cpp \
           apihook.cpp \
           ../hspide/ipc_common.cpp \
           ../hspide/filemanager.cpp \
           ../hspide/breakpointmanager.cpp
