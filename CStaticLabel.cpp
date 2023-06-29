// CStaticLabel.cpp : implementation file
//

#include "pch.h"
#include "NameServerLookup.h"
#include "afxdialogex.h"
#include "CStaticLabel.h"


// CStaticLabel dialog

IMPLEMENT_DYNAMIC(CStaticLabel, CDialogEx)

CStaticLabel::CStaticLabel(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NAMESERVERLOOKUP_DIALOG, pParent)
{

}

CStaticLabel::~CStaticLabel()
{
}

void CStaticLabel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStaticLabel, CDialogEx)
END_MESSAGE_MAP()


// CStaticLabel message handlers
