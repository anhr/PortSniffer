// EditBannerURL.h: interface for the CEditBannerURL class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include <atlctrls.h>

class CEditBannerURL : public CWindowImpl<CEditBannerURL, CEdit>
{
public:
	CEditBannerURL()
	{
	}
	virtual ~CEditBannerURL()
		{}

	DECLARE_WND_SUPERCLASS(NULL, CEdit::GetWndClassName())

	void Create(HWND hWndParent, ATL::_U_MENUorID MenuOrID)
	{
		CRect rect(10, 2, 600, 24);
		HWND hWnd = 0;
		hWnd = CWindowImpl<CEditBannerURL, CEdit>::Create(hWndParent
				, &rect//ATL::_U_RECT rect = NULL
				, NULL//LPCTSTR szWindowName = NULL
				,	//CBS_DROPDOWN | CBS_AUTOHSCROLL | CBS_SORT
					ES_AUTOHSCROLL | ES_READONLY
					| WS_CHILD | WS_TABSTOP | WS_VISIBLE// | WS_BORDER | WS_VSCROLL//DWORD dwStyle = 0
				, 0//WS_EX_CLIENTEDGE - for border //DWORD dwExStyle = 0
				,	MenuOrID//ATL::_U_MENUorID MenuOrID = 0U
				, NULL//LPVOID lpCreateParam = NULL
			);
		ATLASSERT(hWnd);
		ATLASSERT(::IsWindow(m_hWnd));
		SetFont(AtlGetDefaultGuiFont());
//SetWindowText(_T("http://bonalink.com/banner.srf"));

		// Setup the tooltip control for tooltips with the static controls
		m_ToolTip.Create(m_hWnd);
		CString strToolTip;
		strToolTip.LoadString(IDS_BANNER_URL);//Banner URL
		CWindow wnd;
//#pragma warning(disable : 4311)
		{
			CToolInfo ti(TTF_IDISHWND | TTF_SUBCLASS | TTF_ABSOLUTE// | TTF_PARSELINKS
				, m_hWnd,  (UINT)(m_hWnd),  NULL, (LPTSTR)(LPCTSTR)strToolTip);
			m_ToolTip.AddTool(&ti);
		}
/*
		{
//			strToolTip.LoadString(IDS_CMB_PHONE_NUMBER);//Phone number.
			wnd.m_hWnd = GetWindow(GW_CHILD);
			while(wnd.m_hWnd)
			{
				CToolInfo ti(TTF_IDISHWND | TTF_SUBCLASS | TTF_ABSOLUTE
					, wnd.m_hWnd,  (UINT)(wnd.m_hWnd),  NULL, (LPTSTR)(LPCTSTR)strToolTip);
				m_ToolTip.AddTool(&ti);
				wnd.m_hWnd = wnd.GetWindow(GW_HWNDNEXT);
			}
		}
*/
		m_ToolTip.SetDelayTime(1000, 0);
		m_ToolTip.Activate(TRUE);
	}

	CEdit& operator=(HWND hWnd)
	{
		ATLVERIFY(SubclassWindow(hWnd));
		ATLASSERT(m_hWnd && ::IsWindow(m_hWnd));
		return *this;
	}
/*
	CString GetWindowText()
	{
		CComBSTR bstr;
		BOOL res = CWindowImpl<CEditBannerURL, CEdit>::GetWindowText(&bstr);
		ATLVERIFY(res);
		CString str = bstr;
		return str;
	}
*/
private:

BEGIN_MSG_MAP(CEditBannerURL)
	{
		CString strError;
		try
		{
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
			::AfxMessageBox(_T("ERROR: CEditBannerURL failed! ") + strError, MB_ICONSTOP);
		}
	}
END_MSG_MAP()

	CToolTipCtrl m_ToolTip;

};
