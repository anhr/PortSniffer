// PaneContainerBanner.h : interface of the CPaneContainerBanner class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <atlctrls.h>
#include <atlctrlx.h>
#include "ToolBarCtrlBanner.h"
#include <EXDISPID.H>//for DISPID_STATUSTEXTCHANGE

#define CONTROL_ID 123

class CPaneContainerBanner :
	public CPaneContainerImpl<CPaneContainerBanner>
  , public IDispEventImpl<CONTROL_ID, CPaneContainerBanner>
{
public:
	CPaneContainerBanner(void);
	virtual ~CPaneContainerBanner(void);
	void Create(HWND hWndParent);
	void CalcSize();
	void AdviseSinkMap()
	{ ::AtlAdviseSinkMap(this, true); }

	BEGIN_MSG_MAP(CPaneContainerBanner)
		{
			CString strError;
			try
			{
				MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
				MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
//				FORWARD_NOTIFICATIONS()
				CHAIN_MSG_MAP(CPaneContainerImpl<CPaneContainerBanner>)
				CHAIN_MSG_MAP_MEMBER(m_ctlToolBarBanner)
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
				::AfxMessageBox(_T("ERROR: CMainFrame failed! ") + strError, MB_ICONEXCLAMATION);
			}
		}
	END_MSG_MAP()

	BEGIN_SINK_MAP(CPaneContainerBanner)
		SINK_ENTRY(CONTROL_ID, DISPID_STATUSTEXTCHANGE, OnStatusTextChange)
		SINK_ENTRY(CONTROL_ID, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
	END_SINK_MAP()

protected:
private:
	CToolBarCtrlBanner m_ctlToolBarBanner;

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;//1;   // no background needed
	}
/*
	LRESULT ForwardNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lResult = 0;
		if(IsWindow())
			bHandled = FALSE;
*/
/*
		switch(uMsg)
		{
		case WM_COMMAND:
		case WM_NOTIFY:
		case WM_PARENTNOTIFY:
		case WM_DRAWITEM:
		case WM_MEASUREITEM:
		case WM_COMPAREITEM:
		case WM_DELETEITEM:
		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL:
		case WM_VSCROLL:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
			lResult = GetParent().SendMessage(uMsg, wParam, lParam);
			break;
		default:
			bHandled = FALSE;
			break;
		}
*/
/*
		return lResult;
	}
*/
	VOID __stdcall OnStatusTextChange(BSTR bstrStatusText)
	{
		CString strStatusText = bstrStatusText;
//		if(!strStatusText.IsEmpty())
		{
			_Module.Status(strStatusText);
		}
	}

	VOID __stdcall OnDocumentComplete(IDispatch* pDisp, VARIANT* pvtURL)
	{
		ATLASSERT(pvtURL && (pvtURL->vt == VT_BSTR));
		CString strURL(pvtURL->bstrVal);
		m_ctlToolBarBanner.SetURL(strURL);
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
//		m_ctlToolBarBanner.DestroyWindow();
		::AtlAdviseSinkMap(this, false);
		return 0;
	}
};
