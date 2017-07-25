// ServerManagerDoc.h : interface of the CServerManagerDoc class
//


#pragma once


class CServerManagerDoc : public CDocument
{
protected: // create from serialization only
	CServerManagerDoc();
	DECLARE_DYNCREATE(CServerManagerDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CServerManagerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


