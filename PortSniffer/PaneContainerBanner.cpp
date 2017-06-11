//PaneContainerBanner.cpp, implementation of the CPaneContainerBanner class.
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PaneContainerBanner.h"

//////////////////////////////////////////////////////////////////////
// Enable debug memory manager
#ifdef _DEBUG
#ifdef THIS_FILE
#undef THIS_FILE
#endif //THIS_FILE
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

CPaneContainerBanner::CPaneContainerBanner(void)
	: m_ctlToolBarBanner(this)
{
}
CPaneContainerBanner::~CPaneContainerBanner(void)
{
}

void CPaneContainerBanner::Create(HWND hWndParent)
{
	ATLVERIFY(CPaneContainerImpl<CPaneContainerBanner>::Create(hWndParent, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN));
	ATLASSERT(IsWindow());
	HWND hWndToolBarListen = m_ctlToolBarBanner.Create(m_hWnd, m_cxyBorder + m_cxyBtnOffset, m_cxyBorder + m_cxyBtnOffset);//, boTransparent, boDropDownArrows);
}

void CPaneContainerBanner::CalcSize()
{
	m_cxyHeader = 24 //CDlgPortScanner height
		+ m_cxyBorder + m_cxyBtnOffset;
}

