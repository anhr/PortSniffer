// ToolBarCtrlBanner.h: interface for the CToolBarCtrlBanner class.
//

#pragma once

#include "MyToolBarCtrl.h"
#include "DlgPortScanner.h"
#include "AfxMessageBox.h"
#include "EditBannerURL.h"

/////////////////////////////////////////////////////////////////////////////
// CToolBarCtrlBanner class

class CToolBarCtrlBanner : public CMyToolBarCtrl
{
public:
	CToolBarCtrlBanner(CWindow* pMainFrame);

	HWND Create(HWND hWndParent, int x = 0, int y = 0);//, BOOL boTransparent = TRUE, BOOL boDropDownArrows = TRUE);
//	void SetButtonState(int identifier, bool boListening);
//	void DestroyWindow()
//	{ ATLVERIFY(CMyToolBarCtrl::DestroyWindow()); }
	void SetURL(LPCTSTR lpszURL)
	{ ATLVERIFY(m_editURL.SetWindowText(lpszURL));}

BEGIN_MSG_MAP(CToolBarCtrlBanner)
	{
		CString strError;
		try
		{
			CHAIN_MSG_MAP(CMyToolBarCtrl)

			//Attention!!! Include this line before CHAIN_MSG_MAP_MEMBER(m_editURL) for calling of the CDlgPortScanner::OnDestroy(...) function once only
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

			CHAIN_MSG_MAP_MEMBER(m_editURL)
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
			::AfxMessageBox(_T("ERROR: CToolBarCtrlBanner failed! ") + strError, MB_ICONSTOP);
		}
	}
END_MSG_MAP()

private:
	static const int m_nButtonCount;
	CEditBannerURL m_editURL;
//	CDlgPortScanner m_dlgPortScanner;
	virtual void Error(LPCTSTR lpszError)
	{ ::AfxMessageBox(lpszError, MB_ICONSTOP);}
	static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
//		bHandled = FALSE;
		return 0;
	}
};
