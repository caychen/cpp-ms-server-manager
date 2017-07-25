// CreateServDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ServerManager.h"
#include "CreateServDlg.h"


// CCreateServDlg dialog

IMPLEMENT_DYNAMIC(CCreateServDlg, CDialog)

CCreateServDlg::CCreateServDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateServDlg::IDD, pParent)
{

}

CCreateServDlg::~CCreateServDlg()
{
}

void CCreateServDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_STARTTYPE, m_StartTypeBox);
}


BEGIN_MESSAGE_MAP(CCreateServDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_BROWSE, &CCreateServDlg::OnBnClickedBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_QUIT, &CCreateServDlg::OnBnClickedBtnQuit)
	ON_BN_CLICKED(IDC_BTN_ADD_SERV, &CCreateServDlg::OnBnClickedBtnAddServ)
END_MESSAGE_MAP()


// CCreateServDlg message handlers

void CCreateServDlg::OnBnClickedBtnBrowse()
{
	// TODO: Add your control notification handler code here
	CFileDialog openFileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		,_T("All Files(*.*)|(*.*)||"));
	if(IDOK == openFileDlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT_BINPATH,openFileDlg.GetPathName());
	}

}

BOOL CCreateServDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_StartTypeBox.InsertString(0,_T("�Զ�"));
	m_StartTypeBox.InsertString(1,_T("�ֶ�"));
	m_StartTypeBox.InsertString(2,_T("����"));
	m_StartTypeBox.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCreateServDlg::OnBnClickedBtnQuit()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CCreateServDlg::OnBnClickedBtnAddServ()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	DWORD dwStartType;
	CString strServName,strServDispName,strBinPath,strStartType,strServDesc;
	GetDlgItemText(IDC_EDIT_SERVNAME,strServName);
	GetDlgItemText(IDC_EDIT_DISPNAME,strServDispName);
	GetDlgItemText(IDC_EDIT_BINPATH,strBinPath);
	GetDlgItemText(IDC_COMBO_STARTTYPE,strStartType);
	GetDlgItemText(IDC_EDIT_SERVDESC,strServDesc);

	if(strServName.IsEmpty())
	{
		AfxMessageBox(_T("�������Ʋ���Ϊ�գ�"));
		return;
	}
	if(strServDispName.IsEmpty())
	{
		AfxMessageBox(_T("��ʾ���Ʋ���Ϊ�գ�"));
		return;
	}
	if(strBinPath.IsEmpty())
	{
		AfxMessageBox(_T("����·������Ϊ�գ�"));
		return;
	}
	if(strStartType.IsEmpty())
	{
		AfxMessageBox(_T("��ѡ��������ʽ��"));
		return;
	}
	
	if(strStartType == _T("�Զ�"))
		dwStartType = SERVICE_AUTO_START;
	else if(strStartType == _T("�ֶ�"))
		dwStartType = SERVICE_DEMAND_START;
	else
		dwStartType = SERVICE_DISABLED;

	CServConfig ServConfig;
	if(ServConfig.CreateServ(strServName,strBinPath,strServDispName,dwStartType,strServDesc))
	{
		AfxMessageBox(_T("��������ɹ���"));
	}
	else
		AfxMessageBox(_T("��������ʧ�ܣ�"));

	OnOK();
}
