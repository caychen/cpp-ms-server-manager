#pragma once
#include "afxwin.h"
#include "ServConfig.h"


// CCreateServDlg dialog

class CCreateServDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateServDlg)

public:
	CCreateServDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCreateServDlg();

// Dialog Data
	enum { IDD = IDD_DLG_ADD_SERV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnBrowse();
public:
	virtual BOOL OnInitDialog();
public:
	CComboBox m_StartTypeBox;
public:
	afx_msg void OnBnClickedBtnQuit();
public:
	afx_msg void OnBnClickedBtnAddServ();
};
