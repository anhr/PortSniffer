//PaneContainerOutput.cpp, implementation of the CPaneContainerOutput class.
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PaneContainerOutput.h"

//////////////////////////////////////////////////////////////////////
// Enable debug memory manager
#ifdef _DEBUG
#ifdef THIS_FILE
#undef THIS_FILE
#endif //THIS_FILE
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

CPaneContainerOutput::CPaneContainerOutput(void)
	: m_ctlToolBarListen(this)
//	, m_hWndToolBarListen(NULL)
{
}
CPaneContainerOutput::~CPaneContainerOutput(void)
{
}

void CPaneContainerOutput::Create(HWND hWndParent)
{
	ATLVERIFY(CPaneContainerImpl<CPaneContainerOutput>::Create(hWndParent, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN));
	ATLASSERT(IsWindow());
/*
}

void CPaneContainerOutput::CreateCloseButton()
{
*/
	HWND hWndToolBarListen = m_ctlToolBarListen.Create(m_hWnd, m_cxyBorder + m_cxyBtnOffset, m_cxyBorder + m_cxyBtnOffset);//, boTransparent, boDropDownArrows);
/*
	CRect rectToolBarListen;
	ATLVERIFY(::GetWindowRect(hWndToolBarListen, &rectToolBarListen));
	m_ctlToolBarListen.SetWindowPos(NULL, m_cxyBorder + m_cxyBtnOffset, m_cxyBorder + m_cxyBtnOffset, rectToolBarListen.Width(), m_cyImageTB + m_cxyBtnAddTB + 10, SWP_NOZORDER | SWP_NOACTIVATE);
*/
/*
	ATLASSERT(m_tb.m_hWnd == NULL);
	// create toolbar for the "x" button
	m_tb.Create(m_hWnd, rcDefault, NULL, 
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT
		| TBSTYLE_LIST//for transparent
//		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
//		| CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN | CCS_NOMOVEY
//		| TBSTYLE_TOOLTIPS | TBSTYLE_FLAT
		, 0);
	ATLASSERT(m_tb.IsWindow());
//	m_tb.SetNotifyWnd(NULL);

	if(m_tb.m_hWnd != NULL)
	{
//		T* pT = static_cast<T*>(this);
//		pT;   // avoid level 4 warning
		CString string;
		int nStringLength;
		TCHAR * pString;
		int nIndex = -1;

		m_tb.SetButtonStructSize();

		TBBUTTON tbbtn = { 0 };

		nIndex = 0;
		tbbtn.idCommand = m_nCloseBtnID;
		tbbtn.fsState = TBSTATE_ENABLED;
		tbbtn.fsStyle = TBSTYLE_BUTTON;
		m_tb.AddButtons(1, &tbbtn);

		//Listen button
		nIndex++;
//		ATLASSERT(m_nButtonCount > nIndex);
		string.LoadString(IDS_LISTEN);//&Listen
		nStringLength = string.GetLength() + 1;
		pString = string.GetBufferSetLength(nStringLength);
		pString[nStringLength] = 0;
		ATLVERIFY(m_tb.InsertButton(nIndex, ID_LISTEN//int iCommand = &Listaen. Start or stop listening of the port\nToggle listening
			, TBSTYLE_CHECK//BYTE Style TBSTYLE_BUTTON;// | (boDropDownArrows ? TBSTYLE_DROPDOWN : 0);
			, TBSTATE_ENABLED//BYTE State | TBSTATE_CHECKED;//TBSTATE_INDETERMINATE
			, -1//int iBitmap
	    , m_tb.AddStrings(pString)//INT_PTR iString
			, NULL//DWORD_PTR lParam
			));

		m_tb.SetBitmapSize(m_cxImageTB, m_cyImageTB);
		m_tb.SetButtonSize((m_cxImageTB + m_cxyBtnAddTB) * 3, m_cyImageTB + m_cxyBtnAddTB);
/*
		if(IsVertical())
			m_tb.SetWindowPos(NULL, m_cxyBorder + m_cxyBtnOffset, m_cxyBorder + m_cxyBtnOffset, m_cxImageTB + m_cxyBtnAddTB, m_cyImageTB + m_cxyBtnAddTB, SWP_NOZORDER | SWP_NOACTIVATE);
		else
		{
			m_tb.SetWindowPos(NULL, 0, 0, (m_cxImageTB + m_cxyBtnAddTB) * 2, m_cyImageTB + m_cxyBtnAddTB, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
//			m_tb.SetWindowPos(NULL, 0, 0, m_cxImageTB + m_cxyBtnAddTB, m_cyImageTB + m_cxyBtnAddTB, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}
*/
/*
		m_tb.SetWindowPos(NULL, m_cxyBorder + m_cxyBtnOffset, m_cxyBorder + m_cxyBtnOffset, (m_cxImageTB + m_cxyBtnAddTB) * 2, m_cyImageTB + m_cxyBtnAddTB, SWP_NOZORDER | SWP_NOACTIVATE);
	}
*/
}

//LRESULT CPaneContainerOutput::OnNotify(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
/*
{
	if(m_tb.m_hWnd == NULL)
	{
		bHandled = FALSE;
		return 1;
	}

//	T* pT = static_cast<T*>(this);
//	pT;
	LPNMHDR lpnmh = (LPNMHDR)lParam;
	LRESULT lRet = 0;

	// pass toolbar custom draw notifications to the base class
	if(lpnmh->code == NM_CUSTOMDRAW && lpnmh->hwndFrom == m_tb.m_hWnd)
		lRet = CPaneContainerImpl<CPaneContainerOutput>::OnCustomDraw(lpnmh->idFrom, lpnmh, bHandled);
#ifndef _WIN32_WCE
	// tooltip notifications come with the tooltip window handle and button ID,
	// pass them to the parent if we don't handle them
	else if(lpnmh->code == TTN_GETDISPINFO && lpnmh->idFrom == m_nCloseBtnID)
		bHandled = GetToolTipText(lpnmh);
#endif // !_WIN32_WCE
	// only let notifications not from the toolbar go to the parent
	else if(lpnmh->hwndFrom != m_tb.m_hWnd && lpnmh->idFrom != m_nCloseBtnID)
		bHandled = FALSE;

	return lRet;
}
*/

void CPaneContainerOutput::CalcSize()
{
/*
	CRect rectToolBarListen;
	ATLASSERT(::IsWindow(m_hWndToolBarListen));
	ATLVERIFY(::GetWindowRect(m_hWndToolBarListen, &rectToolBarListen));
*/
	m_cxyHeader = 24 //CDlgPortScanner height
		+ m_cxyBorder + m_cxyBtnOffset;
}

