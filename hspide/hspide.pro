######################################################################
# Automatically generated by qmake (2.01a) ? 7 22 01:02:55 2011
######################################################################

TEMPLATE = vcapp
TARGET = 
DEPENDPATH += . debug release i18n
INCLUDEPATH += .
QT += xml network


# ランタイムライブラリを静的リンク 
# Qtライブラリ側も同じオプションでビルドされていること！
QMAKE_CXXFLAGS_DEBUG  -= -MDd
QMAKE_CXXFLAGS_DEBUG  += -MTd
QMAKE_CXXFLAGS_RELEASE-= -MD
QMAKE_CXXFLAGS_RELEASE+= -MT
QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:"msvcrt.lib"
QMAKE_LFLAGS_DEBUG   += /NODEFAULTLIB:"msvcrtd.lib"

# Input
HEADERS += outputdock.h debuggerdock.h projectdock.h searchdock.h symboldock.h messagedock.h \
           documentpane.h mainwindow.h workspaceitem.h workspacemodel.h compiler.h debugger.h \
           codeedit.h newfiledialog.h jumpdialog.h debuggercommand.hpp
SOURCES += main.cpp \
           outputdock.cpp debuggerdock.cpp projectdock.cpp searchdock.cpp symboldock.cpp messagedock.cpp \
           documentpane.cpp mainwindow.cpp workspaceitem.cpp workspacemodel.cpp compiler.cpp debugger.cpp \
           codeedit.cpp newfiledialog.cpp jumpdialog.cpp
RESOURCES += hspide.qrc
TRANSLATIONS += i18n/hspide_ja.ts
FORMS    = newfiledialog.ui jumpdialog.ui
