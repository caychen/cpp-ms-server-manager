#include "StdAfx.h"
#include "ServConfig.h"

CServConfig::CServConfig(void)
{
}

CServConfig::~CServConfig(void)
{
}

CServItem* CServConfig::EnumServList()
{
	BOOL bRet;
	SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(!hSCM)
		return NULL;
	
	LPENUM_SERVICE_STATUS pServStatus = NULL;
	DWORD dwBytesNeeded = 0,dwServReturned = 0,dwResumeHandle = 0,dwRealBytes = 0;
	bRet = EnumServicesStatus(hSCM,SERVICE_WIN32,SERVICE_STATE_ALL,
		pServStatus,0,&dwBytesNeeded,&dwServReturned,&dwResumeHandle);

	//ERROR_MORE_DATA
	/*There are more service entries than would fit into the lpServices buffer.
	**The actual number of service entries written to lpServices is returned in the lpServicesReturned parameter.
	**The number of bytes required to get the remaining entries is returned in the pcbBytesNeeded parameter.
	**The remaining services can be enumerated by additional calls to 
	**		EnumServicesStatus with the lpResumeHandle parameter indicating the next service to read.
	*/
	if(!bRet && GetLastError() == ERROR_MORE_DATA)
	{
		dwRealBytes = dwBytesNeeded;
		pServStatus = new ENUM_SERVICE_STATUS[dwRealBytes + 1];
		ZeroMemory(pServStatus,dwRealBytes + 1);
		bRet = EnumServicesStatus(hSCM,SERVICE_WIN32,SERVICE_STATE_ALL,
			pServStatus,dwRealBytes,&dwBytesNeeded,&dwServReturned,&dwResumeHandle);

		if(!bRet)
		{
			CloseServiceHandle(hSCM);
			return NULL;
		}
	}
	else
	{
		CloseServiceHandle(hSCM);
		return NULL;
	}

	CServItem *pServHeader = NULL,*pServPre = NULL,*pServNext = NULL;
	pServPre = pServNext;
	for(DWORD dwIdx = 0;dwIdx < dwServReturned;++dwIdx)
	{
		pServNext = new CServItem;
		pServNext->m_strServName = pServStatus[dwIdx].lpServiceName;
		pServNext->m_strServDispName = pServStatus[dwIdx].lpDisplayName;
		pServNext->m_dwServStatus = pServStatus[dwIdx].ServiceStatus.dwCurrentState;
		GetServPathAndStartType(pServNext->m_strServName,*pServNext);
		pServNext->m_strDescription = GetServDescription(pServNext->m_strServName);
		(pServHeader == NULL) ? (pServHeader = pServNext) : pServHeader;
		(pServPre == NULL) ? (pServPre = pServNext) : 
			(pServPre->m_pNext = pServNext,pServPre = pServNext);
	}

	CloseServiceHandle(hSCM);
	delete []pServStatus;
	return pServHeader;
}

CString CServConfig::GetStatusString(DWORD dwCurrentStatus)
{
	CString strResult;
	switch(dwCurrentStatus)
	{
	case SERVICE_STOPPED:
		strResult = _T("");
		break;

	case SERVICE_START_PENDING:
		strResult = _T("正在启动");
		break;

	case SERVICE_STOP_PENDING:
		strResult = _T("正在停止");
		break;

	case SERVICE_RUNNING:
		strResult = _T("已启动");
		break;

	case SERVICE_CONTINUE_PENDING:
		strResult = _T("继续中");
		break;

	case SERVICE_PAUSE_PENDING:
		strResult = _T("暂停中");
		break;;

	case SERVICE_PAUSED:
		strResult = _T("暂停");
		break;	
	}
	return strResult;
}

BOOL CServConfig::GetServPathAndStartType(LPCTSTR lpszServName,CServItem &tItem)
{
	BOOL bRet = FALSE;
	SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(hSCM == NULL)
		return FALSE;
	
	SC_HANDLE hSrv = OpenService(hSCM,lpszServName,SERVICE_QUERY_CONFIG);
	if(hSrv == NULL)
	{
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	QUERY_SERVICE_CONFIG *pSerConfig = NULL;
	DWORD cbBuffSize = 0,cbBytesNeeded = 0;
	bRet = QueryServiceConfig(hSrv,NULL,0,&cbBytesNeeded);
	if(!bRet && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		pSerConfig = new QUERY_SERVICE_CONFIG[cbBytesNeeded + 1];
		ZeroMemory(pSerConfig,cbBytesNeeded + 1);
		cbBuffSize = cbBytesNeeded;
		cbBytesNeeded = 0;
	}
	else
	{
		goto __Error_End;
	}

	bRet = QueryServiceConfig(hSrv,pSerConfig,cbBuffSize,&cbBytesNeeded);
	if(!bRet)
	{
		goto __Error_End;
	}

	tItem.m_strBinPath = pSerConfig->lpBinaryPathName;
	tItem.m_dwStartType = pSerConfig->dwStartType;
	bRet = TRUE;

__Error_End:
	CloseServiceHandle(hSrv);
	CloseServiceHandle(hSCM);
	return bRet;
}

CString CServConfig::GetStartTypeString(DWORD dwStartType)
{
	CString strResult;
	switch(dwStartType)
	{
	case SERVICE_AUTO_START:
		strResult = _T("自动");
		break;

	case SERVICE_DEMAND_START:
		strResult = _T("手动");
		break;

	case SERVICE_DISABLED:
		strResult = _T("已禁用");
		break;

	default:
		strResult = _T("未知");
		break;
	}
	return strResult;
}

CString CServConfig::GetServDescription(LPCTSTR lpszServName)
{
	CString strResult;
	BOOL bRet = FALSE;
	SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(hSCM == NULL)
		return strResult;

	SC_HANDLE hSrv = OpenService(hSCM,lpszServName,SERVICE_QUERY_CONFIG);
	if(hSrv == NULL)
	{
		CloseServiceHandle(hSCM);
		return strResult;
	}

	LPSERVICE_DESCRIPTION pDescription = NULL;
	DWORD dwNeeded = 0,dwLen = 0;
	bRet = QueryServiceConfig2(hSrv,SERVICE_CONFIG_DESCRIPTION,NULL,0,&dwNeeded);
	if(!bRet && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		dwLen = dwNeeded + 1;
		pDescription = new SERVICE_DESCRIPTION[dwLen];
		ZeroMemory(pDescription,dwLen);
		dwNeeded = 0;

		if(QueryServiceConfig2(hSrv,SERVICE_CONFIG_DESCRIPTION,(LPBYTE)pDescription,dwLen,&dwNeeded))
		{
			strResult = pDescription->lpDescription;
		}
	}

	CloseServiceHandle(hSrv);
	CloseServiceHandle(hSCM);
	return strResult;
}

BOOL CServConfig::CtrlServStatus(LPCTSTR lpszServName,DWORD dwNewStatus)
{
	BOOL bRet = FALSE;
	SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(hSCM == NULL)
		return FALSE;

	SC_HANDLE hSrv = OpenService(hSCM,lpszServName,SERVICE_ALL_ACCESS);
	if(hSrv == NULL)
	{
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	SERVICE_STATUS sTs = {0};
	if(!QueryServiceStatus(hSrv,&sTs))
	{
		goto __Error_End;
	}

	if(sTs.dwCurrentState == dwNewStatus)
		return TRUE;
	else if(sTs.dwCurrentState == SERVICE_START_PENDING ||
		sTs.dwCurrentState == SERVICE_STOP_PENDING ||
		sTs.dwCurrentState == SERVICE_START_PENDING ||
		sTs.dwCurrentState == SERVICE_CONTINUE_PENDING ||
		sTs.dwCurrentState == SERVICE_PAUSE_PENDING)
		return FALSE;

	else if(sTs.dwCurrentState == SERVICE_STOPPED && dwNewStatus == SERVICE_RUNNING)
		bRet = StartService(hSrv,NULL,NULL);
	else if((sTs.dwCurrentState == SERVICE_RUNNING || sTs.dwCurrentState == SERVICE_PAUSED) && dwNewStatus == SERVICE_STOPPED)
		bRet = ControlService(hSrv,SERVICE_CONTROL_STOP,&sTs);
	else if(sTs.dwCurrentState == SERVICE_PAUSED && dwNewStatus == SERVICE_RUNNING)
		bRet = ControlService(hSrv,SERVICE_CONTROL_CONTINUE,&sTs);
	else if(sTs.dwCurrentState == SERVICE_RUNNING && dwNewStatus == SERVICE_PAUSED)
		bRet = ControlService(hSrv,SERVICE_CONTROL_PAUSE,&sTs);
		
__Error_End:
	CloseServiceHandle(hSrv);
	CloseServiceHandle(hSCM);
	return bRet;
}

long CServConfig::GetServCtrlAccepted(LPCTSTR lpszServName,DWORD *pdwCurrStatus /* = NULL */)
{
	long dwCtrlAccepted = -1;
	SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(hSCM == NULL)
		return -1;

	SC_HANDLE hSrv = OpenService(hSCM,lpszServName,SERVICE_ALL_ACCESS);
	if(hSrv == NULL)
	{
		CloseServiceHandle(hSCM);
		return -1;
	}

	SERVICE_STATUS sTs = {0};
	if(!QueryServiceStatus(hSrv,&sTs))
	{
		goto __Error_End;
	}

	dwCtrlAccepted = sTs.dwControlsAccepted;
	if(pdwCurrStatus)
		*pdwCurrStatus = sTs.dwCurrentState;

__Error_End:
	CloseServiceHandle(hSrv);
	CloseServiceHandle(hSCM);
	return dwCtrlAccepted;
}

BOOL CServConfig::CtrlServStartType(LPCTSTR lpszServName,DWORD dwNewStartType)
{
	BOOL bRet = FALSE;
	SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(hSCM == NULL)
		return FALSE;

	SC_HANDLE hSrv = OpenService(hSCM,lpszServName,SERVICE_ALL_ACCESS);
	if(hSrv == NULL)
	{
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	CServItem ServItem;
	if(GetServPathAndStartType(lpszServName,ServItem) 
		&& ServItem.m_dwStartType == dwNewStartType)
	{
		bRet = TRUE;
		goto __Error_End;
	}

	bRet = ChangeServiceConfig(hSrv,SERVICE_NO_CHANGE,dwNewStartType,SERVICE_NO_CHANGE,
		NULL,NULL,NULL,NULL,NULL,NULL,NULL);

__Error_End:
	CloseServiceHandle(hSrv);
	CloseServiceHandle(hSCM);
	return bRet;
}

BOOL CServConfig::DeleteServ(LPCTSTR lpszServName)
{
	BOOL bRet = FALSE;
	SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(hSCM == NULL)
		return FALSE;

	SC_HANDLE hSrv = OpenService(hSCM,lpszServName,SERVICE_ALL_ACCESS);
	if(hSrv == NULL)
	{
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	if(CtrlServStatus(lpszServName,SERVICE_STOPPED))
		bRet = DeleteService(hSrv);

	CloseServiceHandle(hSrv);
	CloseServiceHandle(hSCM);
	return bRet;
}

BOOL CServConfig::CreateServ(LPCTSTR lpszServName, LPCTSTR lpszBinPath, LPCTSTR lpszDispName, DWORD dwStartType, LPCTSTR lpszDesc, LPCTSTR lpszPass)
{
	BOOL bRet = FALSE;
	SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(hSCM == NULL)
		return FALSE;

	CString strDispName,strPass;
	if(!lpszDispName)
		strDispName = lpszServName;
	else
		strDispName = lpszDispName;

	SC_HANDLE hSrv = CreateService(hSCM,lpszServName,strDispName,SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,dwStartType,SERVICE_ERROR_NORMAL,
		lpszBinPath,NULL,NULL,NULL,NULL,lpszPass);
	if(hSrv && lpszDesc)
	{
		bRet = TRUE;
		TCHAR szDesc[MAX_PATH] = {0};
		SERVICE_DESCRIPTION sServDesc = {0};
		_tcscpy_s(szDesc,MAX_PATH,lpszDesc);
		sServDesc.lpDescription = szDesc;
		ChangeServiceConfig2(hSrv,SERVICE_CONFIG_DESCRIPTION,&sServDesc);
		CloseServiceHandle(hSrv);
	}

	CloseServiceHandle(hSCM);
	return bRet;
}

void CServConfig::FreeServList(CServItem *pItem)
{
	CServItem *pT = NULL;
	while(pItem)
	{
		pT = pItem->m_pNext;
		delete pItem;
		pItem = pT;
	}
}