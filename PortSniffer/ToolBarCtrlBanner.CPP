//ToolBarCtrlBanner.CPP, implementation of the CToolBarCtrlBanner class.

#include "stdafx.h"
#include "ToolBarCtrlBanner.h"
#include "resource.h"
#include <atlframe.h>//for ATL_SIMPLE_TOOLBAR_PANE_STYLE

//////////////////////////////////////////////////////////////////////
// Enable debug memory manager
#ifdef _DEBUG
#ifdef THIS_FILE
#undef THIS_FILE
#endif //THIS_FILE
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

const int CToolBarCtrlBanner::m_nButtonCount = 1;

CToolBarCtrlBanner::CToolBarCtrlBanner(CWindow* pMainFrame)
	: CMyToolBarCtrl(pMainFrame)
{
}

HWND CToolBarCtrlBanner::Create(HWND hWndParent, int x, int y)//, BOOL boTransparent, BOOL boDropDownArrows)
{
	HWND hWnd = CMyToolBarCtrl::Create(hWndParent
		, ATL_SIMPLE_TOOLBAR_PANE_STYLE
		| TBSTYLE_LIST//for transparent
		);
	ATLASSERT(::IsWindow(hWnd));

	CString string;
	int nIndex = -1;
//	int nStringLength;
//	TCHAR * pString;
	CMyToolBarCtrl::m_pTBButtons = new TBBUTTON[m_nButtonCount];

	//URL
	nIndex++;
	ATLASSERT(m_nButtonCount > nIndex);
	m_pTBButtons[nIndex].iString = -1;
	m_pTBButtons[nIndex].fsState = 0;//TBSTATE_ENABLED;
	m_pTBButtons[nIndex].fsStyle = TBSTYLE_BUTTON;//TBSTYLE_SEP;
	m_pTBButtons[nIndex].dwData = 0;
	m_pTBButtons[nIndex].iBitmap = -1;
	m_pTBButtons[nIndex].idCommand = WMU_BANNER_URL;
	m_editURL.Create(m_hWnd, WMU_BANNER_URL);
	ATLASSERT(m_editURL.IsWindow());
	CRect rectEditURL;
	ATLVERIFY(m_editURL.GetWindowRect(&rectEditURL));

	CMyToolBarCtrl::SetButtonStructSize();
	ATLVERIFY(CMyToolBarCtrl::AddButtons(m_nButtonCount, m_pTBButtons));
	CMyToolBarCtrl::SetButtonWidth(WMU_BANNER_URL, rectEditURL.Width());

	if(x || y)
	{
		CRect rectToolBar;
		ATLVERIFY(::GetWindowRect(m_hWnd, &rectToolBar));
		CRect rectEditURL;
		ATLVERIFY(m_editURL.GetWindowRect(&rectEditURL));
		ATLVERIFY(CMyToolBarCtrl::SetWindowPos(NULL, x, y, rectToolBar.Width(), rectEditURL.Height(), SWP_NOZORDER | SWP_NOACTIVATE));
	}

	return m_hWnd;
}

VOID CALLBACK CToolBarCtrlBanner::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	ATLASSERT(m_pnTimer);
	ATLVERIFY(::KillTimer(NULL, m_pnTimer));
	m_pnTimer = NULL;
}
/*
void CToolBarCtrlBanner::SetButtonState(int identifier, bool boListening)
{
	if(!::IsWindow(m_hWnd))
		return;

	TBBUTTONINFO tbButtonInfo;
	::ZeroMemory(&tbButtonInfo, sizeof(TBBUTTONINFO));
	tbButtonInfo.cbSize = sizeof(TBBUTTONINFO);
	tbButtonInfo.dwMask = TBIF_STATE;// | TBIF_SIZE;// | TBIF_TEXT | TBIF_IMAGE
	BOOL res = FALSE;
	tbButtonInfo.fsState = boListening ? TBSTATE_ENABLED | TBSTATE_CHECKED : TBSTATE_ENABLED;
	ATLASSERT(identifier);
	res = CMyToolBarCtrl::SetButtonInfo(identifier, &tbButtonInfo);
	ATLVERIFY(res);
	InvalidateParent();

	m_dlgPortScanner.Enable(boListening);
}
*/