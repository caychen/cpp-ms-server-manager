// ServerManagerView.h : interface of the CServerManagerView class
//


#pragma once
#include "ServConfig.h"
#include "CreateServDlg.h"


class CServerManagerView : public CListView
{
protected: // create from serialization only
	CServerManagerView();
	DECLARE_DYNCREATE(CServerManagerView)

// Attributes
public:
	CServerManagerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CServerManagerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStartService();
	afx_msg void OnStopService();
	afx_msg void OnPauseService();
	afx_msg void OnContinueService();
	afx_msg void OnAutoStart();
	afx_msg void OnDemandStart();
	afx_msg void OnDisabled();
	afx_msg void OnAddService();
	afx_msg void OnDeleteService();
	afx_msg void OnMenuRefresh();
	afx_msg void OnDestroy();

public:
	CServItem *m_pServList;
	
public:
	afx_msg void OnRunningService();
};

#ifndef _DEBUG  // debug version in ServerManagerView.cpp
inline CServerManagerDoc* CServerManagerView::GetDocument() const
   { return reinterpret_cast<CServerManagerDoc*>(m_pDocument); }
#endif

