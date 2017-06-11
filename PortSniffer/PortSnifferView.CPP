//PortSnifferView.CPP, implementation of the CPortSnifferView class.

#include "StdAfx.h"
#include "PortSnifferView.h"
#include "SocketBlocking\SocketListening.h"
#include "RegKeyPortScanner.h"


//////////////////////////////////////////////////////////////////////
// Enable debug memory manager
#ifdef _DEBUG
#ifdef THIS_FILE
#undef THIS_FILE
#endif //THIS_FILE
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

void CPortSnifferView::Listen(bool boQuitApp)
{
	if(IsListening())
	{//stop listening
//		CWaitCursor w;
		m_socketListening.Stop(boQuitApp);
		_Module.Status(IDS_LISTEN_STOP);//The listening was stopped
		if(!boQuitApp)
			_Module.RefreshBanner();
	}
	else
	{//start listening
		CString strError;
		UINT nPort = 0;
		try
		{
			CString strHostAddress;
			_Module.GetHost(&strHostAddress, &nPort);
			m_socketListening.Listen(nPort);
			strError = m_socketListening.GetError();
			_Module.Status(IDS_LISTEN_START);//Start listening
		}
		catch(CException* e)
		{
			strError = e->GetErrorMessage();
			delete e;
		}
		catch(...)
		{
			strError = _T("Unhandled exception!!!");
		}
		if(!strError.IsEmpty())
		{
			CString strMessage;
			strMessage.Format(_T("ERROR: CPortSnifferView::ThreadListen() failed! %s\nport = %d"), strError, nPort);
			::AfxMessageBox(strMessage, MB_ICONSTOP, _Module.GetMainFrame()->m_hWnd);
			ATLTRACE(strError + _T("\n"));
			ATLASSERT(FALSE);
		}
		else return;//Success start listening
	}
}

LRESULT CPortSnifferView::OnListen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& bHandled)
{
	bHandled = FALSE;

	Listen(hWndCtl ? false : true);//if hWndCtl == 0 - close aplication

	return 0;
}

LRESULT CPortSnifferView::OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	HWND hWndFocus = GetFocus();
	if(hWndFocus == m_hWnd)
		CRichEditCtrl::Copy();
	else 
		bHandled = FALSE;
	return 0;
}

LRESULT CPortSnifferView::OnEditSelectall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CRichEditCtrl::SetSelAll();
	return 0;
}

