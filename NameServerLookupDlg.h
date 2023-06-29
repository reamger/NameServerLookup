
// NameServerLookupDlg.h : header file
//

#pragma once


// CNameServerLookupDlg dialog
class CNameServerLookupDlg : public CDialogEx
{
// Construction
public:
	CNameServerLookupDlg(CWnd* pParent = nullptr);	// standard constructor
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NAMESERVERLOOKUP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();

private:
	CFont m_font;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedLookup();
	CString m_nameserver;
	CString m_domain;
	CString m_resultv4;
	CString m_resultv6;
	CNSLookup NsLookup;
	in_addr addrBuf4;
	in6_addr addrBuf6;
	ADDRINFOW addrinfo;
	PADDRINFOW paddrinfoRes;
	int iResult;
};
