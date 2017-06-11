//SocketListeningPortScanner.cpp

#include "StdAfx.h"
#include "SocketListeningPortScanner.h"
//#include "WaitCursor.h"

//////////////////////////////////////////////////////////////////////
// Enable debug memory manager
#ifdef _DEBUG
#ifdef THIS_FILE
#undef THIS_FILE
#endif //THIS_FILE
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

CSocketListeningPortScanner::CSocketListeningPortScanner(void)
{
}

CSocketListeningPortScanner::~CSocketListeningPortScanner(void)
{
	Stop();
}

bool CSocketListeningPortScanner::Stop(bool boQuitApp)
{
//	CWaitCursor w;
	CSocketListening<CSocketAccept>::Stop(false);
	return CSocketClient::WaitArrayClientsEmpty(boQuitApp);
/*
	switch (CSocketClient::WaitArrayClientsEmpty())
	{
	case WAIT_OBJECT_0:
		break;
	case WAIT_TIMEOUT:
	case WAIT_ABANDONED:
	case WAIT_FAILED:
	default: ATLASSERT(FALSE);
	}
	_Module.Status(IDS_LISTEN_STOP);//The listening was stopped
*/
}
