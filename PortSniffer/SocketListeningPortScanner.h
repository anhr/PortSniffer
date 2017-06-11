//SocketListeningPortScanner.h

#pragma once

#include "SocketAccept.h"
#include "SocketBlocking\SocketListening.h"

class CSocketAcceptWeb;

class CSocketListeningPortScanner :
	private  CSocketListening<CSocketAccept>
{
public:
	CSocketListeningPortScanner(void);
	virtual ~CSocketListeningPortScanner(void);
	void Listen(int nPort)
	{ CSocketListening<CSocketAccept>::Listen(nPort);}
	CString GetError()
	{ return CSocketListening<CSocketAccept>::GetError();}
//	void Stop();
	bool Stop(bool boQuitApp = false);
	bool IsOpen()
	{ return CSocketListening<CSocketAccept>::IsOpen(); } 
private:
	virtual void Add(CSocketAccept& sConnect)
	{
		CSocketListening<CSocketAccept>::Add(sConnect);
		CString strStatus;
		strStatus.Format(IDS_SESSION_OPEN, CSocketListening<CSocketAccept>::GetCount());//%ld session(s) open
		_Module.Status(strStatus, 0);
	}
};
