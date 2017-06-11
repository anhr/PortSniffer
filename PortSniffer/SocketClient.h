// SocketClient.h : interface of the CAboutDlg class

#pragma once

#include "socketblocking\socketblocking.h"
#include "MyCriticalSection.h"
#include "ResponseFile.h"

class CSocketAccept;

class CSocketClient :
	private CSocketTelnet
{
public:
	CSocketClient(CSocketAccept* psocketAccept);
	virtual ~CSocketClient(void);
	bool Start();
	void Stop(DWORD dwMilliseconds = INFINITE)
	{
		if(!CSocketTelnet::IsThreadCreated())
			return;
		CSocketTelnet::Stop(dwMilliseconds);

		//ERROR: The wait operation timed out.
		//CSocketTelnet::WaitForCloseThread();
	}
	int Send(const char* pch, const int nSize)
	{ return CSocketBlocking::Send(pch, nSize); }
	void SetSocketAccept(CSocketAccept* psocketAccept = NULL)
	{
//ATLTRACE(_T("1 CSocketClient::m_psocketAccept = 0x%x\n"), m_psocketAccept);
		m_psocketAccept = psocketAccept;
//ATLTRACE(_T("2 CSocketClient::m_psocketAccept = 0x%x\n"), m_psocketAccept);
	}
	bool IsOpen()
	{ return CSocketTelnet::IsOpen(); } 
	bool IsThreadCreated()
	{ return CSocketTelnet::IsThreadCreated(); }
	bool IsDeleteThis()
	{ return m_boDeleteThis;}
	void SetResponseFileName(LPCTSTR lpszResponseFileName);
	void CloseResponseFileName();
	LPCTSTR GetHostAddress()
	{ return m_strHostAddress; }
private:
	CSocketAccept* m_psocketAccept;
	bool m_boDeleteThis;
	CResponseFile m_file;
	CString m_strResponseFileName;
//CString m_strResponseTail;
	bool m_boSaveResponseToFile;
//	bool m_boSaveResponseToFile2;
	bool m_boFirstEmptyLine;
	CString m_strHostAddress;
	enum 
	{
		content_type_unknown//0
		, content_type_text//1
	} m_contentType;

	//Clients array
	static UINT m_nClientsCount;
	static bool m_boQuitApp;

public:
	static bool WaitArrayClientsEmpty(bool boQuitApp)
	{
		m_boQuitApp = boQuitApp;
		if(m_nClientsCount == 0)
			return false;
		return true;
	}
private:

	CString RelayToAccept(int nRetCode, char* buf);
	CString Receive()
	{ return RelayToAccept(CSocketTelnet::ReceiveBuf(), CSocketTelnet::GetBufReceive()); }
	bool IsSaveResponseToFile()
	{ return m_strResponseFileName.IsEmpty() ? false : true;}
	virtual void OnReceive(int nErrorCode);
	void OnReceiveCS(int nErrorCode);
	virtual bool ReceiveLine(LPCTSTR lpszResponse);
	virtual void OnClose(int nErrorCode);
	virtual void DeleteThis();
};
