
// NameServerLookup.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "NameServerLookup.h"
#include "NameServerLookupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNameServerLookupApp

BEGIN_MESSAGE_MAP(CNameServerLookupApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNameServerLookupApp construction

CNameServerLookupApp::CNameServerLookupApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CNameServerLookupApp object

CNameServerLookupApp theApp;


// CNameServerLookupApp initialization

BOOL CNameServerLookupApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CNameServerLookupDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

CNSLookup::CNSLookup() : 
	Socketv4(INVALID_SOCKET),
	Socketv6(INVALID_SOCKET),
	query_id(1)
{}

int CNSLookup::socketInit()
{
	WSAData wsaData;
	int ret;
	DWORD tv = 2000;	//2s
	int successSocket = 0, iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %d\n", iResult);
		return successSocket;
	}
	// Create a SOCKET for connecting to server
	Socketv4 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (Socketv4 == INVALID_SOCKET)
		wprintf(L"socketv4 failed with error: %ld\n", WSAGetLastError());
	else
	{
		successSocket++;
		ret = setsockopt(Socketv4, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(timeval));
		// TRACE("ret1 = %d\n");
		ret = setsockopt(Socketv4, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(timeval));
		// TRACE("ret2 = %d\n");
	}	
	Socketv6 = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (Socketv6 == INVALID_SOCKET)
		wprintf(L"socketv6 failed with error: %ld\n", WSAGetLastError());
	else 
	{
		successSocket++;
		ret = setsockopt(Socketv6, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(timeval));
		// TRACE("ret3 = %d\n");
		ret = setsockopt(Socketv6, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(timeval));
		// TRACE("ret4 = %d\n");
	}
	if (successSocket == 0)
		WSACleanup();
	return successSocket;
}

BOOL CNSLookup::v4lookup(CString& result4, CString& result6)
{
	SOCKADDR_IN nsv4;
	int sendlength = 0, ret = 0;
	struct sockaddr from;
	IN_ADDR rawResultv4;
	IN6_ADDR rawResultv6;
	int fromAddrSize = sizeof(from);
	char sendbuf[1024] = "", recvbuf[1024] = "";
	nsv4.sin_family = AF_INET;
	nsv4.sin_port = htons(53);
	nsv4.sin_addr = m_server4;

	makeQuery(AF_INET, sendbuf, sendlength);
	sendto(Socketv4, sendbuf, sendlength, 0, (struct sockaddr*)&nsv4, sizeof(nsv4));
	ret = recvfrom(Socketv4, recvbuf, 1023, 0, &from, &fromAddrSize);
	if (ret == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err == WSAETIMEDOUT)
			result4 = "Connection timed out";
		else if (err == WSAECONNREFUSED)
			result4 = "Connection refused";
		else 
		{
			result4 = "socketv4 failed with error code %d";
			result4.Format(result4, err);
		}
			
		// wprintf(L"socketv4 failed with error: %ld\n", err);
	}
	else {
		ret = decodev4Answer(recvbuf, ret, rawResultv4);
		if (ret == 1) {
			result4 = "No such record";
		}
		else if (ret == 10)
			result4 = "Record exists but server cannot answer";
		else if (ret == 0)
		{
			wchar_t resBuf[16] = L"";
			if (InetNtop(AF_INET, &rawResultv4, resBuf, 16) == NULL)
				result4 = "Parse dns reply failed";
			else
				result4 = resBuf;

		}
		else if (ret == -2)
			result4 = "Answer out of order";
		else if (ret == -1)
			result4 = "Unknow error: reply is too short";
	}


	makeQuery(AF_INET6, sendbuf, sendlength);
	sendto(Socketv4, sendbuf, sendlength, 0, (struct sockaddr*)&nsv4, sizeof(nsv4));
	ret = recvfrom(Socketv4, recvbuf, 1023, 0, &from, &fromAddrSize);
	if (ret == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err == WSAETIMEDOUT)
			result6 = "Connection timed out";
		else if (err == WSAECONNREFUSED)
			result6 = "Connection refused";
		else
		{
			result6 = "socketv4 failed with error code %d";
			result6.Format(result6, err);
		}
		// wprintf(L"socketv4 failed with error: %ld\n", err);
	}
	else {
		ret = decodev6Answer(recvbuf, ret, rawResultv6);
		if (ret == 1) {
			result6 = "No such record";
		}
		else if (ret == 10)
			result6 = "Record exists but server cannot answer";
		else if (ret == 0)
		{
			wchar_t resBuf[46] = L"";
			if (InetNtop(AF_INET6, &rawResultv6, resBuf, 46) == NULL)
				result6 = "Parse dns reply failed";
			else
				result6 = resBuf;
		}
		else if (ret == -2)
			result6 = "Answer out of order";
		else if (ret == -1)
			result6 = "Unknow error: reply is too short";
	}
	return TRUE;
}

BOOL CNSLookup::v6lookup(CString& result4, CString& result6)
{
	SOCKADDR_IN6 nsv6;
	int sendlength = 0, ret = 0;
	SOCKADDR_IN6 from;
	IN_ADDR rawResultv4;
	IN6_ADDR rawResultv6;
	int fromAddrSize = sizeof(from);
	// TRACE("%d", fromAddrSize);
	char sendbuf[1024] = "", recvbuf[1024] = "";
	nsv6.sin6_family = AF_INET6;
	nsv6.sin6_port = htons(53);
	nsv6.sin6_addr = m_server6;
	nsv6.sin6_scope_id = 0;

	makeQuery(AF_INET, sendbuf, sendlength);
	sendto(Socketv6, sendbuf, sendlength, 0, (struct sockaddr*)&nsv6, sizeof(nsv6));
	ret = recvfrom(Socketv6, recvbuf, 1023, 0, (SOCKADDR*)&from, &fromAddrSize);
	if (ret == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err == WSAETIMEDOUT)
			result4 = "Connection timed out";
		else if (err == WSAECONNREFUSED)
			result4 = "Connection refused";
		else
		{
			result4 = "socketv6 failed with error code %d";
			result4.Format(result4, err);
		}
		// wprintf(L"socketv6 failed with error: %ld\n", err);
	}
	else {
		ret = decodev4Answer(recvbuf, ret, rawResultv4);
		if (ret == 1) {
			result4 = "No such record";
		}
		else if (ret == 10)
			result4 = "Record exists but server cannot answer";
		else if (ret == 0)
		{
			wchar_t resBuf[16] = L"";
			if (InetNtop(AF_INET, &rawResultv4, resBuf, 16) == NULL)
				result4 = "Parse dns reply failed";
			else
				result4 = resBuf;

		}
		else if (ret == -2)
			result4 = "Answer out of order";
		else if (ret == -1)
			result4 = "Unknow error: reply is too short";
	}


	makeQuery(AF_INET6, sendbuf, sendlength);
	sendto(Socketv6, sendbuf, sendlength, 0, (struct sockaddr*)&nsv6, sizeof(nsv6));
	ret = recvfrom(Socketv6, recvbuf, 1023, 0, (SOCKADDR*)&from, &fromAddrSize);
	if (ret == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err == WSAETIMEDOUT)
			result6 = "Connection timed out";
		else if (err == WSAECONNREFUSED)
			result6 = "Connection refused";
		else
		{
			result6 = "socketv6 failed with error code %d";
			result6.Format(result6, err);
		}
		// wprintf(L"socketv6 failed with error: %ld\n", err);
	}
	else {
		ret = decodev6Answer(recvbuf, ret, rawResultv6);
		if (ret == 1) {
			result6 = "No such record";
		}
		else if (ret == 10)
			result6 = "Record exists but server cannot answer";
		else if (ret == 0)
		{
			wchar_t resBuf[46] = L"";
			if (InetNtop(AF_INET6, &rawResultv6, resBuf, 46) == NULL)
				result6 = "Parse dns reply failed";
			else
				result6 = resBuf;
		}
		else if (ret == -2)
			result6 = "Answer out of order";
		else if (ret == -1)
			result6 = "Unknow error: reply is too short";
	}
	return TRUE;
}

BOOL CNSLookup::makeQuery(INT Family, char* sendbuf, int& sendlength)
{
	CString domain = m_domain;
	int length = -1, pidx = 0;
	sendlength = 12;
	char headbuf[12] = { query_id >>8, query_id };		//id(2B)
	headbuf[2] = 0x01; headbuf[3] = 0x00;	//flags(2B)
	headbuf[4] = 0x00; headbuf[5] = 0x01;	//Question count(2B)
	headbuf[6] = 0x00; headbuf[7] = 0x00;	//Answer count(2B)
	headbuf[8] = 0x00; headbuf[9] = 0x00;	//Authority record count(2B)
	headbuf[10] = 0x00; headbuf[11] = 0x00;	//Additional record count(2B)

	char nameblock[256] = "";
	memcpy(sendbuf, headbuf, 12);
	pidx = domain.Find(L'.');
	while (pidx != -1) {
		if (pidx == -1)
			break;
		length = WideCharToMultiByte(CP_ACP, 0, domain.Left(pidx), pidx, nameblock, 255, NULL, NULL);
		sendbuf[sendlength] = length;
		sendlength++;
		memcpy(&sendbuf[sendlength], nameblock, length);
		sendlength += length;
		if (length > 0)
			nameblock[length] = 0;
		// TRACE(nameblock);
		// TRACE("\n");
		domain = domain.Mid(pidx + 1);
		pidx = domain.Find(L'.');
	}
	if (!domain.IsEmpty()) {
		length = WideCharToMultiByte(CP_ACP, 0, domain, -1, nameblock, 255, NULL, NULL);
#ifdef DEBUG
		assert(length > 1);
#endif // DEBUG
		length--;	//because of the -1 in WideCharToMultiByte, delete the \0
		sendbuf[sendlength] = length;
		sendlength++;
		memcpy(&sendbuf[sendlength], nameblock, length);
		sendlength += length;
		domain.Empty();
	}
	sendbuf[sendlength] = 0; sendlength += 1;	//handle the root domain 0
	if (Family == AF_INET) {
		sendbuf[sendlength] = 0x00; sendbuf[sendlength + 1] = 0x01; sendlength += 2;	//Type
	}
	else if (Family == AF_INET6) {
		sendbuf[sendlength] = 0x00; sendbuf[sendlength + 1] = 0x1c; sendlength += 2;	//Type
	}
	else
		return FALSE;

	sendbuf[sendlength] = 0x00; sendbuf[sendlength + 1] = 0x01; sendlength += 2;	//Class
	query_id++;
	return TRUE;
}

int CNSLookup::decodev4Answer(char* recvbuf, int length, IN_ADDR& resv4)
{
	if (length < 16)
		return -1;
#ifdef DEBUG
	TRACE("%d\n", MAKEWORD(recvbuf[1], recvbuf[0]));
#endif // DEBUG
	if (MAKEWORD(recvbuf[1], recvbuf[0]) != query_id - 1) {
		return -2;
	}
	if ((recvbuf[3] & 0x0f) == 0x03)
		return 1;
	if (recvbuf[7] == 0x00)
		return 10;
	memcpy(&resv4, &recvbuf[length - 4], 4);
	return 0;
}

int CNSLookup::decodev6Answer(char* recvbuf, int length, IN6_ADDR& resv6)
{
	if (length < 28)
		return -1;
#ifdef DEBUG
	TRACE("%d\n", MAKEWORD(recvbuf[1], recvbuf[0]));
#endif // DEBUG
	if (MAKEWORD(recvbuf[1],recvbuf[0]) != query_id - 1) {
		return -2;
	}
	if ((recvbuf[3] & 0x0f) == 0x03)
		return 1;
	if (recvbuf[7] == 0x00)
		return 10;
	memcpy(&resv6, &recvbuf[length - 16], 16);
	return 0;
}
