#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#pragma once

// éQçl:http://developer.qt.nokia.com/forums/viewthread/10655

//////////////////////////////////////////////////////////////////////
// íËã`

class QtLocalSocket
{
	HANDLE	m_pipe;

public:

	typedef enum OpenMode {
		ReadOnly  = GENERIC_READ,
		WriteOnly = GENERIC_WRITE,
		ReadWrite = GENERIC_READ | GENERIC_WRITE,
	};

public:
	QtLocalSocket();
	virtual ~QtLocalSocket();
	bool connectToServer(const char * name, OpenMode openMode = ReadWrite);
	bool disconnectFromServer();
	unsigned long writeData(const char * data, unsigned long c);
};

//////////////////////////////////////////////////////////////////////
// é¿ëï

inline
QtLocalSocket::QtLocalSocket()
	: m_pipe(NULL)
{
}

inline
QtLocalSocket::~QtLocalSocket()
{
	disconnectFromServer();
}

inline
bool QtLocalSocket::connectToServer(const char * name, OpenMode openMode)
{
	TCHAR pipename[MAX_PATH*2+1] = _T("\\\\.\\pipe\\");
	strcat_s(pipename, name);
	m_pipe = ::CreateFile(pipename, (DWORD)openMode,
	                      NULL, NULL, OPEN_EXISTING, 0, NULL);
	if( INVALID_HANDLE_VALUE == m_pipe ) {
		m_pipe = NULL;
	}
	return NULL != m_pipe;
}

inline
bool QtLocalSocket::disconnectFromServer()
{
	::CloseHandle(m_pipe);
	m_pipe = NULL;
	return true;
}

inline
unsigned long QtLocalSocket::writeData(const char * data, unsigned long c)
{
	DWORD sendLen = 0;
	if( m_pipe &&
		!WriteFile(m_pipe, data, (DWORD)c, &sendLen, NULL) )
	{
		return 0;
	} 
	return sendLen;
}

