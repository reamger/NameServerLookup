
// NameServerLookupDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "NameServerLookup.h"
#include "NameServerLookupDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_LOOKUPDONE (WM_USER + 101)


// CNameServerLookupDlg dialog



CNameServerLookupDlg::CNameServerLookupDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NAMESERVERLOOKUP_DIALOG, pParent)
	, m_nameserver(_T(""))
	, m_domain(_T(""))
	, m_resultv4(_T(""))
	, m_resultv6(_T(""))
	, threadID(GetCurrentThreadId())
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	NsLookup.socketInit();
	addrinfo.ai_addrlen = 0;
	addrinfo.ai_canonname = NULL;
	addrinfo.ai_addr = NULL;
	addrinfo.ai_next = NULL;
}

void CNameServerLookupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SERVER, m_nameserver);
	DDX_Text(pDX, IDC_DOMAIN, m_domain);
	DDX_Text(pDX, IDC_RESULT, m_resultv4);
	DDX_Text(pDX, IDC_RESULT2, m_resultv6);
}

BEGIN_MESSAGE_MAP(CNameServerLookupDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(ID_LOOKUP, &CNameServerLookupDlg::OnBnClickedLookup)
	ON_MESSAGE(WM_LOOKUPDONE, OnLookupDone)
END_MESSAGE_MAP()


// CNameServerLookupDlg message handlers

BOOL CNameServerLookupDlg::OnInitDialog()
{
#ifdef DEBUG
	TRACE("%d\n", __cplusplus);
#endif // DEBUG
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	//m_font.CreatePointFont(100, _T("Microsoft Sans Serif"));	//font

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNameServerLookupDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNameServerLookupDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CNameServerLookupDlg::OnLookupDone(WPARAM w, LPARAM l)
{
	GetDlgItem(IDC_RESULT)->SetWindowTextW(m_resultv4);
	GetDlgItem(IDC_RESULT2)->SetWindowTextW(m_resultv6);
	return 0;
}



HBRUSH CNameServerLookupDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd->GetDlgCtrlID() == IDC_SERVER_STATIC)
	{
		// pDC->SetBkColor(RGB(0, 255, 0));		//background color: green
		// pDC->SetTextColor(RGB(255, 0, 0));	//text color: red
		// pDC->SelectObject(&m_font);				//text font and size
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


void CNameServerLookupDlg::OnBnClickedLookup()
{
	UpdateData(TRUE);
	m_resultv4 = "Looking up...";
	m_resultv6 = "Looking up...";
	UpdateData(FALSE);
	AfxBeginThread(lookup, this);
	
	// UpdateWindow();
}

UINT CNameServerLookupDlg::lookup(LPVOID pParam)
{
	int iResult = 0;
	CNameServerLookupDlg* p = (CNameServerLookupDlg*)pParam;
	p->m_resultv4 = "Unknown error";
	p->m_resultv6 = "Unknown error";
	p->NsLookup.m_domain = p->m_domain;
	if (p->m_domain.GetLength() > 128)
		p->m_domain = p->m_domain.Mid(0, 128);
	iResult = GetAddrInfo((PCWSTR)(p->m_nameserver), NULL, &(p->addrinfo), &(p->paddrinfoRes));
	if (iResult == 0) {
		if (p->paddrinfoRes->ai_family == AF_INET) {
			p->NsLookup.m_server4 = ((SOCKADDR_IN*)(p->paddrinfoRes->ai_addr))->sin_addr;
			p->NsLookup.v4lookup(p->m_resultv4, p->m_resultv6);
		}
		else if (p->paddrinfoRes->ai_family == AF_INET6) {
			p->NsLookup.m_server6 = ((SOCKADDR_IN6*)(p->paddrinfoRes->ai_addr))->sin6_addr;
			p->NsLookup.v6lookup(p->m_resultv4, p->m_resultv6);
		}
	}
	else if (iResult == WSAHOST_NOT_FOUND) {
		p->m_resultv4 = "NameServer host not found";
		p->m_resultv6 = "NameServer host not found";
	}

	p->PostMessageW(WM_LOOKUPDONE);
	// PostThreadMessage(p->threadID, WM_LOOKUPDONE, NULL, NULL);
	return 0;
}
