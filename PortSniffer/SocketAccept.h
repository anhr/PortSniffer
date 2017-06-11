// SocketAccept.h : interface of the CAboutDlg class

#pragma once

#include "socketblocking\SocketBlocking.h"
#include "socketblocking\ArrayListen.h"
#include "resource.h"
#include "SocketClient.h"
#include "AfxMessageBox.h"
#include "RegKeyPortScanner.h"

class CSocketAccept
	: private CSocketTelnet
{
public:
	CSocketAccept(void)
		: CSocketTelnet(false//false - do not assert (debugging breakpoint) during CException 
			)
		, m_nConnectionID(0)
		, m_parrayAccepts(NULL)
		, m_pSocketClient(new CSocketClient(this))
		, m_boReplaceHostHeader(false)
	{
	}

	CSocketAccept(CSocketAccept& sConnect, CArrayListen<CSocketAccept>* parrayAccepts);

	virtual ~CSocketAccept(void);
	void Cleanup()
	{ CSocketBlocking::Cleanup();}
	void Stop(bool boStopClient = true)
	{
		if(m_pSocketClient)
			m_pSocketClient->Stop();
		CSocketBlocking::Stop(1000);
	}
	static void Destroy()
	{
	}
	static void SetArrayAccepts(CArrayListen<CSocketAccept>* parrayAccepts)
	{
//		ATLASSERT(!m_parrayAccepts && parrayAccepts); m_parrayAccepts = (CArrayAccept*)parrayAccepts;
	}
	static void WSAStart()
	{ CSocketTelnet::WSAStart();}
	void WaitForCloseThread()
	{ if(IsOpen()) CSocketTelnet::WaitForCloseThread(); }
	int Send(const char *lpszString, const int nSize);
	CString GetConnectionID();
	bool IsOpen()
	{ return CSocketTelnet::IsOpen(); } 
	void SetSocketClient(CSocketClient* pSocketClient = NULL)
	{ m_pSocketClient = pSocketClient;}
/*
	static void EnterCSOutput()
	{ m_csOutput.EnterCriticalSection(); }
	static void LeaveCSOutput()
	{ m_csOutput.LeaveCriticalSection(); }
*/
private:
	size_t m_nConnectionID;
	CArrayListen<CSocketAccept>* m_parrayAccepts;
//	CMyFile m_file;
	CRegKeyPortScanner m_regKeyPortScanner;
//	static CMyCriticalSection m_csOutput;//не хочу чтобы одновременно несколько нитей писали в окно output

	//Я сделал указатель на CSocketClient* вместо CSocketClient потому что сейчас объект m_pSocketClient удаляется после удаления объекта CSocketAccept
	// потому что сейчас я не жду закрытия нити m_pSocketClient->Proc()
	// потому что я сейчас не могу ожидать закрытия нити m_pSocketClient->Proc()
	// потому что я сейчас не использую отдельную нить для ожидания закрытия нити Proc()
	// потому что в этом случае возникает путаница если одновременно открывать и закрывать несколько соединений
	CSocketClient* m_pSocketClient;
	bool m_boReplaceHostHeader;

	virtual void Proc();
	virtual void OnClose(int nErrorCode);
	virtual int LastError(LPCTSTR lpszMessage, bool boException = true);
	CString Receive();
	virtual void OnReceive(int nErrorCode);
	void OnReceiveCS(int nErrorCode);
	virtual bool ReceiveLine(LPCTSTR lpszResponse);
	virtual void DeleteThis()
	{ delete this;}
	bool IsReceiveLine()
	{ return m_boReplaceHostHeader || m_regKeyPortScanner.IsSaveResponse();}
};
