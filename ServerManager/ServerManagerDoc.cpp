// ServerManagerDoc.cpp : implementation of the CServerManagerDoc class
//

#include "stdafx.h"
#include "ServerManager.h"

#include "ServerManagerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerManagerDoc

IMPLEMENT_DYNCREATE(CServerManagerDoc, CDocument)

BEGIN_MESSAGE_MAP(CServerManagerDoc, CDocument)
END_MESSAGE_MAP()


// CServerManagerDoc construction/destruction

CServerManagerDoc::CServerManagerDoc()
{
	// TODO: add one-time construction code here

}

CServerManagerDoc::~CServerManagerDoc()
{
}

BOOL CServerManagerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CServerManagerDoc serialization

void CServerManagerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CServerManagerDoc diagnostics

#ifdef _DEBUG
void CServerManagerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CServerManagerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CServerManagerDoc commands
