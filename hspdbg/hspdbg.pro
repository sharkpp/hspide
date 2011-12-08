######################################################################
# Automatically generated by qmake (2.01a) ? 12 3 15:01:20 2011
######################################################################

TEMPLATE = vclib
TARGET = hspdbg
DEPENDPATH += . hspsdk
INCLUDEPATH += .
QT += network
QT -= gui
CONFIG -= embed_manifest_dll
CONFIG += dll thread
DEFINES += HSPDBG_EXPORTS QT_NODLL
DEFINES -= QT_DLL
QMAKE_POST_LINK += copy $(TargetPath) $(ProjectDir)\..\\$(ConfigurationName)\hsp3debug.dll
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
           hspsdk/hsp3debug.h \
           hspsdk/hsp3struct.h \
           hspsdk/hspvar_core.h \
           ../hspide/debuggercommand.hpp
SOURCES += dllmain.cpp hspdbg.cpp stdafx.cpp
