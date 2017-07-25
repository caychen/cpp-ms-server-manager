#pragma once
#include <Winsvc.h>

class CServItem
{
public:
	CServItem(void)
	{
		m_dwServStatus = 0;
		m_pNext = NULL;
	}
public:
	CString m_strServName;
	CString m_strServDispName;
	DWORD m_dwServStatus;
	CServItem *m_pNext;

	CString m_strBinPath;
	DWORD m_dwStartType;

	CString m_strDescription;
};

class CServConfig
{
public:
	CServConfig();
	virtual ~CServConfig();

public:
	CServItem *EnumServList(); 
	CString GetStatusString(DWORD dwCurrentStatus);
	BOOL GetServPathAndStartType(LPCTSTR lpszServName,CServItem &tItem);
	CString GetStartTypeString(DWORD dwStartType);
	CString GetServDescription(LPCTSTR lpszServName);
	long GetServCtrlAccepted(LPCTSTR lpszServName,DWORD *pdwCurrStatus = NULL);
	BOOL CtrlServStatus(LPCTSTR lpszServName,DWORD dwNewStatus);
	BOOL CtrlServStartType(LPCTSTR lpszServName,DWORD dwNewStartType);
	BOOL DeleteServ(LPCTSTR lpszServName);
	BOOL CreateServ(LPCTSTR lpszServName,LPCTSTR lpszBinPath,LPCTSTR lpszDispName = NULL,
		DWORD dwStartType = SERVICE_AUTO_START,LPCTSTR lpszDesc = NULL,
		LPCTSTR lpszPass = NULL);

	void FreeServList(CServItem *pItem);
};