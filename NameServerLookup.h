
// NameServerLookup.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <winsock2.h>  
#include <windows.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")


// CNameServerLookupApp:
// See NameServerLookup.cpp for the implementation of this class
//

class CNameServerLookupApp : public CWinApp
{
public:
	CNameServerLookupApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CNameServerLookupApp theApp;

class CNSLookup
{
public:
	CNSLookup::CNSLookup();
	CString m_domain;
	CString m_v4Record, m_v6Record;
	struct in_addr m_server4;
	struct in6_addr m_server6;
	int socketInit();
	BOOL v4lookup(CString &result4, CString &result6);
	BOOL v6lookup(CString& result4, CString& result6);
	BOOL makeQuery(INT Family, char* sendbuf, int& sendlength);
	int decodev4Answer(char* recvbuf, int length, IN_ADDR& resv4);
	int decodev6Answer(char* recvbuf, int length, IN6_ADDR& resv6);
private:
	SOCKET Socketv4, Socketv6;
	WORD query_id;
};