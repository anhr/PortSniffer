//RegKeyPortScanner.h

#pragma once

#include "RegkeyBase.h"
#include <atlctrls.h>//for CComboBox

class CRegKeyPortScanner :
	private  CRegkeyBase
{
public:
	CRegKeyPortScanner(LPCTSTR lpszKeyName = NULL);
	virtual ~CRegKeyPortScanner(void);
	void ComboBoxURLInit(CComboBox* pComboBoxURL);
	void SetHost(LPCTSTR lpszURL, UINT nPortLocal, UINT nPortRemote = 0);
	void Init();

	//For resolving issue, described in http://mail.mainpine.com/cgi-bin/trac.cgi/ticket/102
//	void ConfigureValues ();
	LPCTSTR GetKeyName()
	{	return m_strKeyName;}

	//Replace host header
	bool IsReplaceHostHeader()
	{	return GetDWORD(m_pszValueNameReplaceHostHeader, 0) == 1 ? true : false;}
	void SetReplaceHostHeader(bool boReplaceHostHeader)
	{ SetDWORD(m_pszValueNameReplaceHostHeader, boReplaceHostHeader); }

	//Save response to file
	bool IsSaveResponse()
	{	return GetDWORD(m_pszValueNameSaveResponse, 1) == 1 ? true : false;}
	void SetSaveResponse(bool boSaveResponse)
	{ SetDWORD(m_pszValueNameSaveResponse, boSaveResponse); }

	//Applacation data folder
	CString GetApplacationDataFolder()
	{	return GetString(m_pszValueNameApplacationDataFolder, GetApplicationDataFolderDefault());}
	void SetApplacationDataFolder(LPCTSTR lpszApplacationDataFolder)
	{	SetString(m_pszValueNameApplacationDataFolder, lpszApplacationDataFolder);}

protected:

private:
	static LPCTSTR m_pszKeyName;
	static LPCTSTR m_pszKeyNameServers;// = _T("\\Servers");
	static LPCTSTR m_pszValueNamePortLocal;
	static LPCTSTR m_pszValueNamePortRemote;
	static LPCTSTR m_pszValueNameURL;
	static LPCTSTR m_pszValueNameDefaultServerID;
	static LPCTSTR m_pszValueNameReplaceHostHeader;
	static LPCTSTR m_pszValueNameSaveResponse;
	static LPCTSTR m_pszValueNameApplacationDataFolder;

	CString GetServer(DWORD dwServerID);
	CString GetApplicationDataFolderDefault();

	//URL
	CString GetURL(DWORD dwServerID)
	{	return GetString(m_pszValueNameURL, NULL, GetServer(dwServerID));}
	void SetURL(LPCTSTR lpszURL, DWORD dwServerID)
	{	SetString(m_pszValueNameURL, lpszURL, GetServer(dwServerID));}

	//port local
	DWORD GetPortLocal(DWORD dwServerID)
	{	return GetDWORD(m_pszValueNamePortLocal, 0, GetServer(dwServerID));}
	void SetPortLocal(DWORD dwPortLocal, DWORD dwServerID)
	{ SetDWORD(m_pszValueNamePortLocal, dwPortLocal, GetServer(dwServerID)); }

	//port remote
	DWORD GetPortRemote(DWORD dwServerID)
	{	return GetDWORD(m_pszValueNamePortRemote, 0, GetServer(dwServerID));}
	void SetPortRemote(DWORD dwPortRemote, DWORD dwServerID)
	{ SetDWORD(m_pszValueNamePortRemote, dwPortRemote, GetServer(dwServerID)); }

	//Default ServerID
	DWORD GetDefaultServerID()
	{	return GetDWORD(m_pszValueNameDefaultServerID);}
public:
	void SetDefaultServerID(DWORD dwDefaultServerID)
	{ SetDWORD(m_pszValueNameDefaultServerID, dwDefaultServerID); }
private:

	//Permissions
//	virtual TCHAR* GetSD();
//	virtual bool IsAllPermissions();
};
