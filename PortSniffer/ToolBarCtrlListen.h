// ToolBarCtrlListen.h: interface for the CToolBarCtrlListen class.
//

#pragma once

#include "MyToolBarCtrl.h"
#include "DlgPortScanner.h"
#include "AfxMessageBox.h"

/////////////////////////////////////////////////////////////////////////////
// CToolBarCtrlListen class

class CToolBarCtrlListen : public CMyToolBarCtrl
{
public:
	CToolBarCtrlListen(CWindow* pMainFrame);

	HWND Create(HWND hWndParent, int x = 0, int y = 0);//, BOOL boTransparent = TRUE, BOOL boDropDownArrows = TRUE);
	void SetButtonState(int identifier, bool boListening);
//	void DestroyWindow()
//	{ ATLVERIFY(CMyToolBarCtrl::DestroyWindow()); }
//	BOOL SetWindowPos(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags)
//	{ return CMyToolBarCtrl::SetWindowPos(hWndInsertAfter, x, y, cx, cy, nFlags);}

BEGIN_MSG_MAP(CToolBarCtrlListen)
	{
		CString strError;
		try
		{
			CHAIN_MSG_MAP(CMyToolBarCtrl)

			//Attention!!! Include this line before CHAIN_MSG_MAP_MEMBER(m_dlgPortScanner) for calling of the CDlgPortScanner::OnDestroy(...) function once only
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

			CHAIN_MSG_MAP_MEMBER(m_dlgPortScanner)
		}
		catch(CException* e)
		{
			strError = e->GetErrorMessage();
			delete e;
		}
/*
		catch(CFileException* e)
		{
			e->GetErrorMessage(strError.GetBuffer(255), 255);
			strError.ReleaseBuffer();
			delete e;
		}
*/
		catch(...)
		{
			strError = _T("Unhandled exception!");
		}
		if(!strError.IsEmpty())
		{
			::AfxMessageBox(_T("ERROR: CToolBarCtrlListen failed! ") + strError, MB_ICONSTOP);
		}
	}
END_MSG_MAP()

private:
	static const int m_nButtonCount;
	CDlgPortScanner m_dlgPortScanner;
/*
	BOOL m_boPhoneNumberVisible;
	BOOL m_boAudioVisible;
	CAudioLevelCtrl m_ctrlAudioLevel;
	CCmbPhoneNumber m_cmbPhoneNumber;
	WORD m_wPhoneNumberWidth;
*/
	virtual void Error(LPCTSTR lpszError)
	{ ::AfxMessageBox(lpszError, MB_ICONSTOP);}
	static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
//		bHandled = FALSE;
		return 0;
	}
};

//extern CToolBarCtrlListen* g_pToolBarCtrlListen;

