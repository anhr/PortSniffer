//RegKeyPortScanner.cpp

#include "StdAfx.h"
#include "RegKeyPortScanner.h"
#include "util_RF.h"
#include "shlobj.h"//for SHGetSpecialFolderPath
#include "FileManagement.h"//for MyCreateDirectory
#include "DlgPortScanner.h"

//////////////////////////////////////////////////////////////////////
// Enable debug memory manager
#ifdef _DEBUG
#ifdef THIS_FILE
#undef THIS_FILE
#endif //THIS_FILE
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

#define COMPANY_NAME "AH"

LPCTSTR CRegKeyPortScanner::m_pszKeyName = _T("SOFTWARE\\")
																							_T(COMPANY_NAME)
																							_T("\\Port Sniffer");
LPCTSTR CRegKeyPortScanner::m_pszKeyNameServers = _T("\\Servers");

LPCTSTR CRegKeyPortScanner::m_pszValueNamePortLocal = _T("Port");
LPCTSTR CRegKeyPortScanner::m_pszValueNamePortRemote = _T("Port Remote");
LPCTSTR CRegKeyPortScanner::m_pszValueNameURL = _T("URL");
LPCTSTR CRegKeyPortScanner::m_pszValueNameDefaultServerID = _T("Default Server ID");
LPCTSTR CRegKeyPortScanner::m_pszValueNameReplaceHostHeader = _T("Replace host header");
LPCTSTR CRegKeyPortScanner::m_pszValueNameSaveResponse = _T("Save Response");
LPCTSTR CRegKeyPortScanner::m_pszValueNameApplacationDataFolder = _T("Applacation Data Folder");

CRegKeyPortScanner::CRegKeyPortScanner
		(
			LPCTSTR lpszKeyName//Specifies the name of a key to be created or opened. This name must be a subkey of HKEY_LOCAL_MACHINE.
		)
	: CRegkeyBase(CString(m_pszKeyName) + lpszKeyName)
{
	m_strKeyName = m_strKeyName + _T("\\Config");
}

CRegKeyPortScanner::~CRegKeyPortScanner(void)
{
	Close();
}

CString CRegKeyPortScanner::GetServer(DWORD dwServerID)
{
	CString strDevice;
	strDevice.Format(CString(m_pszKeyNameServers) + _T("\\%ld"), dwServerID);//\\Servers
	return strDevice;
}

void CRegKeyPortScanner::ComboBoxURLInit(CComboBox* pComboBoxURL)
{
	ATLASSERT(pComboBoxURL && pComboBoxURL->m_hWnd && ::IsWindow(pComboBoxURL->m_hWnd));

	//Open key
	CString strKeyName = m_strKeyName + m_pszKeyNameServers;
/*
	REGSAM samDesired = KEY_ALL_ACCESS;
	if(CRegkeyBase::Open(m_hKeyRoot, strKeyName, samDesired) != ERROR_SUCCESS)
		return; //The Servers key is not exists
	ATLASSERT(CRegKey::m_hKey != NULL);
*/
	CRegkeyBase regkey(strKeyName, CRegkeyBase::m_hKeyRoot);
//	REGSAM samDesired = KEY_ALL_ACCESS;
//	if(CRegkeyBase::Open(m_hKeyRoot, strKeyName, samDesired) == ERROR_SUCCESS)
	if(regkey.Open(strKeyName) != ERROR_SUCCESS)
		return; //The Servers key is not exists

	ATLASSERT(regkey.m_hKey != NULL);

	DWORD dwIndex = 0;
	TCHAR szBuffer[256];
	DWORD dwSize = 256;
//	FILETIME time;
//	while (::RegEnumKeyEx(CRegKey::m_hKey, 0, szBuffer, &dwSize, NULL, NULL, NULL, &time)==ERROR_SUCCESS)
	while (regkey.EnumKey(dwIndex, szBuffer, &dwSize)==ERROR_SUCCESS)
	{
		DWORD dwServerID = ::RF_tol(szBuffer);
		int index = pComboBoxURL->AddString(GetURL(dwServerID));
		ATLASSERT(index != CB_ERR);
		pComboBoxURL->SetItemData(index, (DWORD_PTR)new CDlgPortScanner::CItemData(GetPortLocal(dwServerID),	GetPortRemote(dwServerID)));
//		pComboBoxURL->SetItemData(index, (DWORD_PTR)new CDlgPortScanner::CItemData(GetPort(dwServerID),	GetPort(dwServerID)));
		dwSize = 256;
		dwIndex++;
	}

	//Close key
	regkey.Close();

	pComboBoxURL->SetCurSel(GetDefaultServerID());
}

void CRegKeyPortScanner::Init()
{
//ABCDEFGHIJKLMNOPQRSTUVWXYZ   
	SetHost(_T("ftp://"), 21);
	SetHost(_T("http://"), 80);
	SetHost(_T("https://"), 443);
	SetHost(_T("imap."), 143);
	SetHost(_T("news."), 119);
	SetHost(_T("pop."), 110);
	SetHost(_T("smtp."), 25);
	//¬о врем€ соединени€ с сервером нужно убрать им€ протокола из адреса порта. —м. bool CSocketClient::Start()
}

void CRegKeyPortScanner::SetHost(LPCTSTR lpszURL, UINT nPortLocal, UINT nPortRemote)
{
	DWORD dwHostID = 0;

	//Open key
	CString strKeyName = m_strKeyName + m_pszKeyNameServers;
	CRegkeyBase regkey(strKeyName, CRegkeyBase::m_hKeyRoot);
//	REGSAM samDesired = KEY_ALL_ACCESS;
//	if(CRegkeyBase::Open(m_hKeyRoot, strKeyName, samDesired) == ERROR_SUCCESS)
	if(regkey.Open(strKeyName) == ERROR_SUCCESS)
	{
		ATLASSERT(regkey.m_hKey != NULL);

		DWORD dwIndex = 0;
		TCHAR szBuffer[256];
		DWORD dwSize = 256;
//		FILETIME time;
//		while (::RegEnumKeyEx(CRegKey::m_hKey, dwIndex, szBuffer, &dwSize, NULL, NULL, NULL, &time)==ERROR_SUCCESS)
		while (regkey.EnumKey(dwIndex, szBuffer, &dwSize)==ERROR_SUCCESS)
		{
			DWORD dwHostIDCur = ::RF_tol(szBuffer);
			CString strURLCur = GetURL(dwHostIDCur);
			if(strURLCur == lpszURL)
			{//Edit of existing host
				SetPortLocal(nPortLocal, dwHostIDCur);
				SetPortRemote(nPortRemote, dwHostIDCur);
				return;
			}

			if(dwHostID <= dwHostIDCur)
				dwHostID = dwHostIDCur + 1;

			dwSize = 256;
			dwIndex++;
		}
		regkey.Close();
	}
	SetURL(lpszURL, dwHostID);
//	SetPort(nPort, dwHostID);
	SetPortLocal(nPortLocal, dwHostID);
	SetPortRemote(nPortRemote, dwHostID);
}

CString CRegKeyPortScanner::GetApplicationDataFolderDefault()
{
	CString strFolder;
	ATLVERIFY(::SHGetSpecialFolderPath(NULL, strFolder.GetBuffer(MAX_PATH), CSIDL_APPDATA, TRUE) != NOERROR);
	strFolder.ReleaseBuffer();
	strFolder += _T("\\")_T(COMPANY_NAME)_T("\\PortSniffer\\");
	::MyCreateDirectory(strFolder);
	return strFolder;
}
