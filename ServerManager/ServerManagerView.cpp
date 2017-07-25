// ServerManagerView.cpp : implementation of the CServerManagerView class
//

#include "stdafx.h"
#include "ServerManager.h"

#include "ServerManagerDoc.h"
#include "ServerManagerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerManagerView

IMPLEMENT_DYNCREATE(CServerManagerView, CListView)

BEGIN_MESSAGE_MAP(CServerManagerView, CListView)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CServerManagerView::OnNMRclick)
	ON_COMMAND(ID_START_SERVICE, &CServerManagerView::OnStartService)
	ON_COMMAND(ID_STOP_SERVICE, &CServerManagerView::OnStopService)
	ON_COMMAND(ID_PAUSE_SERVICE, &CServerManagerView::OnPauseService)
	ON_COMMAND(ID_CONTINUE_SERVICE, &CServerManagerView::OnContinueService)
	ON_COMMAND(ID_AUTO_START, &CServerManagerView::OnAutoStart)
	ON_COMMAND(ID_DEMAND_START, &CServerManagerView::OnDemandStart)
	ON_COMMAND(ID_DISABLED, &CServerManagerView::OnDisabled)
	ON_COMMAND(ID_ADD_SERVICE, &CServerManagerView::OnAddService)
	ON_COMMAND(ID_DELETE_SERVICE, &CServerManagerView::OnDeleteService)
	ON_COMMAND(ID_MENU_REFRESH, &CServerManagerView::OnMenuRefresh)
	ON_WM_DESTROY()
	ON_COMMAND(ID_RUNNING_SERVICE, &CServerManagerView::OnRunningService)
END_MESSAGE_MAP()

// CServerManagerView construction/destruction

CServerManagerView::CServerManagerView()
{
	// TODO: add construction code here
	m_pServList = NULL;
}

CServerManagerView::~CServerManagerView()
{
}

BOOL CServerManagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}

void CServerManagerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
	//GetListCtrl()
	SetWindowLong(GetListCtrl().m_hWnd,GWL_STYLE,
		GetWindowLong(GetListCtrl().m_hWnd,GWL_STYLE) | LVS_REPORT);//报表样式

	GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);//扩展风格，整行选中和网格线
	GetListCtrl().InsertColumn(0,_T("服务名称"),LVCFMT_LEFT,150);
	GetListCtrl().InsertColumn(1,_T("服务状态"),LVCFMT_LEFT,90);
	GetListCtrl().InsertColumn(2,_T("启动类型"),LVCFMT_LEFT,90);
	GetListCtrl().InsertColumn(3,_T("文件路径"),LVCFMT_LEFT,230);
	GetListCtrl().InsertColumn(4,_T("服务描述"),LVCFMT_LEFT,600);

	CServConfig ServConfig;
	CServItem *pHeader = ServConfig.EnumServList();

	if(!pHeader)
	{
		return ;
	}

	m_pServList = pHeader;
	for(int idx = 0;pHeader != NULL;++idx)
	{
		GetListCtrl().InsertItem(idx,_T(""));//先插入一行

		//设置每行中每列的值
		GetListCtrl().SetItemText(idx,0,pHeader->m_strServDispName);
		GetListCtrl().SetItemText(idx,1,ServConfig.GetStatusString(pHeader->m_dwServStatus));
		GetListCtrl().SetItemText(idx,2,ServConfig.GetStartTypeString(pHeader->m_dwStartType));
		GetListCtrl().SetItemText(idx,3,pHeader->m_strBinPath);
		GetListCtrl().SetItemText(idx,4,pHeader->m_strDescription);

		GetListCtrl().SetItemData(idx,(DWORD_PTR)pHeader);
		pHeader = pHeader->m_pNext;
	}
}


// CServerManagerView diagnostics

#ifdef _DEBUG
void CServerManagerView::AssertValid() const
{
	CListView::AssertValid();
}

void CServerManagerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CServerManagerDoc* CServerManagerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServerManagerDoc)));
	return (CServerManagerDoc*)m_pDocument;
}
#endif //_DEBUG


// CServerManagerView message handlers

void CServerManagerView::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	LPNMITEMACTIVATE pNMItemActive = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if(pNMItemActive->iItem < 0)
		return;

	CMenu menu,*pMenu = NULL;
	menu.LoadMenu(IDR_R_MENU);
	pMenu = menu.GetSubMenu(0);

	CPoint pt;
	GetCursorPos(&pt);

	CServItem *pItem = (CServItem*)GetListCtrl().GetItemData(pNMItemActive->iItem);
	ASSERT(pItem);

	CServConfig ServConfig;
	DWORD dwCurrStatus = 0;
	DWORD dwAccepted = ServConfig.GetServCtrlAccepted(pItem->m_strServName,&dwCurrStatus);
	if(dwAccepted == -1)
	{
		menu.EnableMenuItem(ID_START_SERVICE,MF_GRAYED);
		menu.EnableMenuItem(ID_STOP_SERVICE,MF_GRAYED);
		menu.EnableMenuItem(ID_PAUSE_SERVICE,MF_GRAYED);
		menu.EnableMenuItem(ID_CONTINUE_SERVICE,MF_GRAYED);
		goto __Track_menu;
	}

	if(!(dwAccepted & SERVICE_ACCEPT_PAUSE_CONTINUE))
	{
		menu.EnableMenuItem(ID_PAUSE_SERVICE,MF_GRAYED);
		menu.EnableMenuItem(ID_CONTINUE_SERVICE,MF_GRAYED);
	}
	if(!(dwAccepted & SERVICE_ACCEPT_STOP))
	{
		menu.EnableMenuItem(ID_STOP_SERVICE,MF_GRAYED);
	}

	if(dwCurrStatus == SERVICE_RUNNING)
	{
		menu.EnableMenuItem(ID_START_SERVICE,MF_GRAYED);
		menu.EnableMenuItem(ID_CONTINUE_SERVICE,MF_GRAYED);
	}
	else if(dwCurrStatus == SERVICE_STOPPED)
	{
		menu.EnableMenuItem(ID_STOP_SERVICE,MF_GRAYED);
		menu.EnableMenuItem(ID_PAUSE_SERVICE,MF_GRAYED);
		menu.EnableMenuItem(ID_CONTINUE_SERVICE,MF_GRAYED);
	}
	else if(dwCurrStatus == SERVICE_PAUSED)
	{
		menu.EnableMenuItem(ID_START_SERVICE,MF_GRAYED);
		menu.EnableMenuItem(ID_PAUSE_SERVICE,MF_GRAYED);
	}
	else 
	{
		menu.EnableMenuItem(ID_START_SERVICE,MF_GRAYED);
		menu.EnableMenuItem(ID_STOP_SERVICE,MF_GRAYED);
		menu.EnableMenuItem(ID_PAUSE_SERVICE,MF_GRAYED);
		menu.EnableMenuItem(ID_CONTINUE_SERVICE,MF_GRAYED);
	}

__Track_menu:
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,pt.x,pt.y,this);

	*pResult = 0;
}

void CServerManagerView::OnStartService()
{
	// TODO: Add your command handler code here
	int iSelect = GetListCtrl().GetSelectionMark();
	if(iSelect >= 0)
	{
		CServConfig ServConfig;
		CServItem *pItem = (CServItem*)GetListCtrl().GetItemData(iSelect);
		ASSERT(pItem);

		if(!ServConfig.CtrlServStatus(pItem->m_strServName,SERVICE_RUNNING))
		{
			MessageBox(_T("启动服务失败！"),_T("提示"),MB_OK);
			return;
		}
		GetListCtrl().SetItemText(iSelect,1,_T("已启动"));
	}
}

void CServerManagerView::OnStopService()
{
	// TODO: Add your command handler code here
	int iSelect = GetListCtrl().GetSelectionMark();
	if(iSelect >= 0)
	{
		CServConfig ServConfig;
		CServItem *pItem = (CServItem*)GetListCtrl().GetItemData(iSelect);
		ASSERT(pItem);

		if(!ServConfig.CtrlServStatus(pItem->m_strServName,SERVICE_STOPPED))
		{
			MessageBox(_T("停止服务失败！"),_T("提示"),MB_OK);
			return;
		}
		GetListCtrl().SetItemText(iSelect,1,_T(""));
	}
}

void CServerManagerView::OnPauseService()
{
	// TODO: Add your command handler code here
	int iSelect = GetListCtrl().GetSelectionMark();
	if(iSelect >= 0)
	{
		CServConfig ServConfig;
		CServItem *pItem = (CServItem*)GetListCtrl().GetItemData(iSelect);
		ASSERT(pItem);

		if(!ServConfig.CtrlServStatus(pItem->m_strServName,SERVICE_PAUSED))
		{
			MessageBox(_T("暂停服务失败！"),_T("提示"),MB_OK);
			return;
		}
		GetListCtrl().SetItemText(iSelect,1,_T("暂停"));
	}
}

void CServerManagerView::OnContinueService()
{
	// TODO: Add your command handler code here
	int iSelect = GetListCtrl().GetSelectionMark();
	if(iSelect >= 0)
	{
		CServConfig ServConfig;
		CServItem *pItem = (CServItem*)GetListCtrl().GetItemData(iSelect);
		ASSERT(pItem);

		if(!ServConfig.CtrlServStatus(pItem->m_strServName,SERVICE_RUNNING))
		{
			MessageBox(_T("恢复服务失败！"),_T("提示"),MB_OK);
			return;
		}
		GetListCtrl().SetItemText(iSelect,1,_T("已启动"));
	}
}

void CServerManagerView::OnAutoStart()
{
	// TODO: Add your command handler code here
	int iSelect = GetListCtrl().GetSelectionMark();
	if(iSelect >= 0)
	{
		CServConfig ServConfig;
		CServItem *pItem = (CServItem*)GetListCtrl().GetItemData(iSelect);
		ASSERT(pItem);
		if(!ServConfig.CtrlServStartType(pItem->m_strServName,SERVICE_AUTO_START))
		{
			MessageBox(_T("改变服务启动方式为自动失败！"),_T("提示"),MB_OK);
			return;
		}
		GetListCtrl().SetItemText(iSelect,2,_T("自动"));
	}
}

void CServerManagerView::OnDemandStart()
{
	// TODO: Add your command handler code here
	int iSelect = GetListCtrl().GetSelectionMark();
	if(iSelect >= 0)
	{
		CServConfig ServConfig;
		CServItem *pItem = (CServItem*)GetListCtrl().GetItemData(iSelect);
		ASSERT(pItem);
		if(!ServConfig.CtrlServStartType(pItem->m_strServName,SERVICE_DEMAND_START))
		{
			MessageBox(_T("改变服务启动方式为手动失败！"),_T("提示"),MB_OK);
			return;
		}
		GetListCtrl().SetItemText(iSelect,2,_T("手动"));
	}
}

void CServerManagerView::OnDisabled()
{
	// TODO: Add your command handler code here
	int iSelect = GetListCtrl().GetSelectionMark();
	if(iSelect >= 0)
	{
		CServConfig ServConfig;
		CServItem *pItem = (CServItem*)GetListCtrl().GetItemData(iSelect);
		ASSERT(pItem);
		if(!ServConfig.CtrlServStartType(pItem->m_strServName,SERVICE_DISABLED))
		{
			MessageBox(_T("改变服务启动方式为禁用失败！"),_T("提示"),MB_OK);
			return;
		}
		GetListCtrl().SetItemText(iSelect,2,_T("已禁用"));
	}
}

void CServerManagerView::OnAddService()
{
	// TODO: Add your command handler code here
	CCreateServDlg CreateServDlg;
	CreateServDlg.DoModal();
}

void CServerManagerView::OnDeleteService()
{
	// TODO: Add your command handler code here
	int iSelect = GetListCtrl().GetSelectionMark();
	if(iSelect < 0)
		return;

	int iRet = MessageBox(_T("确定删除此服务？"),_T("提示"),MB_OKCANCEL | MB_ICONWARNING);
	if(iRet != MB_OK)
		return;

	CServItem *pItem = (CServItem*)GetListCtrl().GetItemData(iSelect);
	CServConfig ServConfig;
	if(ServConfig.DeleteServ(pItem->m_strServName))
		GetListCtrl().DeleteItem(iSelect);
	else
		AfxMessageBox(_T("删除失败"));
}

void CServerManagerView::OnMenuRefresh()
{
	// TODO: Add your command handler code here
	CServConfig ServConfig;
	CServItem *pHeader = ServConfig.EnumServList();

	if(!pHeader)
	{
		return ;
	}

	if(m_pServList)
		ServConfig.FreeServList(m_pServList);

	GetListCtrl().DeleteAllItems();

	m_pServList = pHeader;
	for(int idx = 0;pHeader != NULL;++idx)
	{
		GetListCtrl().InsertItem(idx,_T(""));//先插入一行

		//设置每行中每列的值
		GetListCtrl().SetItemText(idx,0,pHeader->m_strServDispName);
		GetListCtrl().SetItemText(idx,1,ServConfig.GetStatusString(pHeader->m_dwServStatus));
		GetListCtrl().SetItemText(idx,2,ServConfig.GetStartTypeString(pHeader->m_dwStartType));
		GetListCtrl().SetItemText(idx,3,pHeader->m_strBinPath);
		GetListCtrl().SetItemText(idx,4,pHeader->m_strDescription);

		GetListCtrl().SetItemData(idx,(DWORD_PTR)pHeader);
		pHeader = pHeader->m_pNext;
	}
}

void CServerManagerView::OnDestroy()
{
	CListView::OnDestroy();

	// TODO: Add your message handler code here
	CServConfig ServConfig;
	if(m_pServList)
		ServConfig.FreeServList(m_pServList);
}

void CServerManagerView::OnRunningService()
{
	// TODO: Add your command handler code here
	CServConfig ServConfig;
	CServItem *pHeader = ServConfig.EnumServList();

	if(!pHeader)
		return;

	if(m_pServList)
		ServConfig.FreeServList(m_pServList);

	m_pServList = pHeader;
	GetListCtrl().DeleteAllItems();

	for(int idx = 0;pHeader != NULL;)
	{
		if(pHeader->m_dwServStatus == SERVICE_RUNNING)
		{
			GetListCtrl().InsertItem(idx,_T(""));//先插入一行

			//设置每行中每列的值
			GetListCtrl().SetItemText(idx,0,pHeader->m_strServDispName);
			GetListCtrl().SetItemText(idx,1,ServConfig.GetStatusString(pHeader->m_dwServStatus));
			GetListCtrl().SetItemText(idx,2,ServConfig.GetStartTypeString(pHeader->m_dwStartType));
			GetListCtrl().SetItemText(idx,3,pHeader->m_strBinPath);
			GetListCtrl().SetItemText(idx,4,pHeader->m_strDescription);

			GetListCtrl().SetItemData(idx,(DWORD_PTR)pHeader);
			++idx;
		}
		pHeader = pHeader->m_pNext;
	}
}
