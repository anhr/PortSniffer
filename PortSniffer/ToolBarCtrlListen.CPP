//ToolBarCtrlListen.CPP, implementation of the CToolBarCtrlListen class.

#include "stdafx.h"
#include "ToolBarCtrlListen.h"
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

const int CToolBarCtrlListen::m_nButtonCount = 2;

CToolBarCtrlListen::CToolBarCtrlListen(CWindow* pMainFrame)
	: CMyToolBarCtrl(pMainFrame)
{
}

HWND CToolBarCtrlListen::Create(HWND hWndParent, int x, int y)//, BOOL boTransparent, BOOL boDropDownArrows)
{
//	CMyToolBarCtrl::SetTransparent(TRUE);
	HWND hWnd = CMyToolBarCtrl::Create(hWndParent
		, ATL_SIMPLE_TOOLBAR_PANE_STYLE
		| TBSTYLE_LIST//for transparent
/*
		, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
		| CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN
		| TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_LIST
*/
		);
	ATLASSERT(::IsWindow(hWnd));

	CString string;
	int nIndex = -1;
	int nStringLength;
	TCHAR * pString;
	CMyToolBarCtrl::m_pTBButtons = new TBBUTTON[m_nButtonCount];

	int nWidthDlgPortScanner = m_dlgPortScanner.Create(m_hWnd, m_pTBButtons, &nIndex
#ifdef _DEBUG
			, m_nButtonCount
#endif //_DEBUG
		);

	//Listen button
	nIndex++;
	ATLASSERT(m_nButtonCount > nIndex);
	string.LoadString(IDS_LISTEN);//&Listen
	nStringLength = string.GetLength() + 1;
	pString = string.GetBufferSetLength(nStringLength);
	pString[nStringLength] = 0;
	ATLVERIFY((m_pTBButtons[nIndex].iString = AddStrings(pString)) != -1);
	string.ReleaseBuffer();
	m_pTBButtons[nIndex].fsState = TBSTATE_ENABLED;// | TBSTATE_CHECKED;//TBSTATE_INDETERMINATE
	m_pTBButtons[nIndex].fsStyle = TBSTYLE_CHECK;//TBSTYLE_BUTTON;// | (boDropDownArrows ? TBSTYLE_DROPDOWN : 0);
	m_pTBButtons[nIndex].dwData = 0;
	m_pTBButtons[nIndex].iBitmap = -1;
	m_pTBButtons[nIndex].idCommand = ID_LISTEN;//&Listaen. Start or stop listening of the port\nToggle listening

	CMyToolBarCtrl::SetButtonStructSize();
//	CMyToolBarCtrl::SetButtonSize(20, 50);
	ATLVERIFY(CMyToolBarCtrl::AddButtons(m_nButtonCount, m_pTBButtons));
	CMyToolBarCtrl::SetButtonWidth(WMU_DLG_PORT_SCANNER, nWidthDlgPortScanner);

	if(x || y)
	{
		CRect rectToolBarListen;
		ATLVERIFY(::GetWindowRect(m_hWnd, &rectToolBarListen));
		CRect rectPortScanner;
		ATLVERIFY(m_dlgPortScanner.GetWindowRect(&rectPortScanner));
		ATLVERIFY(CMyToolBarCtrl::SetWindowPos(NULL, x, y, rectToolBarListen.Width(), rectPortScanner.Height(), SWP_NOZORDER | SWP_NOACTIVATE));
	}

	return m_hWnd;
}

VOID CALLBACK CToolBarCtrlListen::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	ATLASSERT(m_pnTimer);
	ATLVERIFY(::KillTimer(NULL, m_pnTimer));
	m_pnTimer = NULL;
}

void CToolBarCtrlListen::SetButtonState(int identifier, bool boListening)
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
