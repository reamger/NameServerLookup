
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


// CNameServerLookupDlg dialog



CNameServerLookupDlg::CNameServerLookupDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NAMESERVERLOOKUP_DIALOG, pParent)
	, m_nameserver(_T(""))
	, m_domain(_T(""))
	, m_resultv4(_T(""))
	, m_resultv6(_T(""))
	, iResult(0)
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
	UpdateWindow();
	m_resultv4 = "N/A";
	m_resultv6 = "N/A";
	iResult = 0;
	// TODO: Add your control notification handler code here
	NsLookup.m_domain = m_domain;
	// char* p = (char*)(LPCTSTR)m_nameserver;
	// TRACE(m_nameserver);
	if (m_domain.GetLength() > 128)
		m_domain = m_domain.Mid(0, 128);
	iResult = GetAddrInfo((PCWSTR)m_nameserver, NULL, &addrinfo, &paddrinfoRes);
	if (iResult == 0) {
		if (paddrinfoRes->ai_family == AF_INET) {
			NsLookup.m_server4 = ((SOCKADDR_IN*)(paddrinfoRes->ai_addr))->sin_addr;
			NsLookup.v4lookup(m_resultv4, m_resultv6);
			UpdateData(FALSE);
			return;
		}
		else if (paddrinfoRes->ai_family == AF_INET6) {
			NsLookup.m_server6 = ((SOCKADDR_IN6*)(paddrinfoRes->ai_addr))->sin6_addr;
			NsLookup.v6lookup(m_resultv4, m_resultv6);
			UpdateData(FALSE);
			return;
		}
	}
	UpdateData(FALSE);
}
