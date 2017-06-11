// BannerView.h : interface of the CBannerView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "GetLastErrorString.h"
//#include <EXDISPID.H>//for DISPID_STATUSTEXTCHANGE

//#define CONTROL_ID 123

class CBannerView : public CWindowImpl<CBannerView, CAxWindow>
//  , public IDispEventImpl<CONTROL_ID, CBannerView>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
			return FALSE;

		// give HTML page a chance to translate this message
		return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
	}

	void Create(HWND hWndParent)
	{
/*
		CString strAppName;
		strAppName.LoadString(IDR_MAINFRAME);//Port Sniffer
		CWindowImpl<CBannerView, CAxWindow>::Create(hWndParent, rcDefault
			, _T("http://")
#ifdef DEBUG
//				_T("localhost")
				_T("bonalink.com")
#else //DEBUG
				_T("bonalink.com")
#endif //DEBUG
				_T("/banner.srf?object=") + strAppName
			, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL
			, WS_EX_CLIENTEDGE
			, CONTROL_ID//_U_MENUorID MenuOrID = 0U
//			, LPVOID lpCreateParam = NULL
			);
*/
		CWindowImpl<CBannerView, CAxWindow>::Create(hWndParent, rcDefault
			, GetURL()
			, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL
			, WS_EX_CLIENTEDGE
			, CONTROL_ID//_U_MENUorID MenuOrID = 0U
//			, LPVOID lpCreateParam = NULL
			);
		CComPtr<IUnknown> spUnk;
		::AtlAxGetControl(m_hWnd,&spUnk);
		ATLASSERT(spUnk);
		HRESULT hRet = spUnk->QueryInterface(__uuidof(IWebBrowser2), (void**)&m_pBrowserApp);
		if(FAILED(hRet))
			throw new CException(_T("AtlAxGetControl failed! ") + ::GetLastErrorString(hRet));
		ATLASSERT(m_pBrowserApp);
/*
		HRESULT res = IDispEventImpl<CONTROL_ID, CBannerView>::DispEventAdvise(spUnk);
		ATLVERIFY(SUCCEEDED(res));
//		ATLVERIFY(SUCCEEDED((IDispEventImpl<CONTROL_ID, CBannerView>::DispEventAdvise(m_pBrowserApp))));
*/
/*
CComBSTR bstrURL;
ATLVERIFY(SUCCEEDED(m_pBrowserApp->get_LocationURL(&(bstrURL.m_str))));
*/
	}

	BEGIN_MSG_MAP(CBannerView)
		{
			CString strError;
			try
			{
//				MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
				MESSAGE_HANDLER(WMU_REFRESH_BANNER, OnRefresh)
			}
			catch(CException* e)
			{
				strError = e->GetErrorMessage();
				delete e;
			}
			catch(...)
			{
				strError = _T("Unhandled exception!");
			}
			if(!strError.IsEmpty())
			{
				::AfxMessageBox(_T("ERROR: BEGIN_MSG_MAP(CBannerView) failed! ") + strError, MB_ICONEXCLAMATION);
			}
		}
	END_MSG_MAP()
/*
	BEGIN_SINK_MAP(CBannerView)
		SINK_ENTRY(CONTROL_ID, DISPID_STATUSTEXTCHANGE, OnStatusTextChange)
//		SINK_ENTRY(CONTROL_ID, DISPID_DOWNLOADCOMPLETE, OnDownloadComplete)
	END_SINK_MAP()
*/
private:
	CComQIPtr<IWebBrowser2> m_pBrowserApp;

	CString GetURL()
	{
		CString strAppName;
		strAppName.LoadString(IDR_MAINFRAME);//Port Sniffer
		return _T("http://")
#ifdef DEBUG
//				_T("localhost")
				_T("bonalink.com")
#else //DEBUG
				_T("bonalink.com")
#endif //DEBUG
				_T("/banner.srf?object=") + strAppName;
	}
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnRefresh(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
//		bHandled = FALSE;
//		m_pBrowserApp->Refresh();
		CString str = GetURL();
		BSTR bstr = str.AllocSysString();

		CComVariant vtEmpty;
		LPCTSTR lpszError = NULL;
		switch(m_pBrowserApp->Navigate(bstr, &vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty))//, &vtHeaders))
		{
		case S_OK: break;
		case E_INVALIDARG: lpszError = _T("One or more parameters are invalid."); break;
		case E_OUTOFMEMORY: lpszError = _T("Out of memory."); break;
		default: lpszError = _T("Unknown error.");
		}
		::SysFreeString(bstr);
		if(lpszError)
			throw new CException("Web Browser Navigate error: " + CString(lpszError));
		return TRUE;
	}
};
