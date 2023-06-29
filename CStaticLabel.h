#pragma once
#include "afxdialogex.h"


// CStaticLabel dialog

class CStaticLabel : public CDialogEx
{
	DECLARE_DYNAMIC(CStaticLabel)

public:
	CStaticLabel(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CStaticLabel();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NAMESERVERLOOKUP_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CFont m_font;
	CStatic m_static;
};
