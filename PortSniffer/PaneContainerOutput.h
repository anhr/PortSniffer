// PaneContainerOutput.h : interface of the CPaneContainerOutput class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <atlctrls.h>
#include <atlctrlx.h>
#include "ToolBarCtrlListen.h"

class CPaneContainerOutput :
	public CPaneContainerImpl<CPaneContainerOutput>
{
public:
	CPaneContainerOutput(void);
	virtual ~CPaneContainerOutput(void);
	void Create(HWND hWndParent);
//	void CreateCloseButton();
	void CalcSize();
	void SetButtonState(int identifier, bool boListening)
	{ m_ctlToolBarListen.SetButtonState(identifier, boListening); }

	BEGIN_MSG_MAP(CPaneContainerOutput)
		{
			CString strError;
			try
			{
//				MESSAGE_HANDLER(WM_PAINT, OnPaint)
//				MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
				MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
				MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)

				//ATTENTION!!! Include line below before CHAIN_MSG_MAP(CPaneContainerImpl<CPaneContainerOutput>) line for ID_EDIT_CUT and ID_EDIT_PASTE commands
				CHAIN_MSG_MAP_MEMBER(m_ctlToolBarListen)

				if(IsWindow())
				{
					CHAIN_MSG_MAP(CPaneContainerImpl<CPaneContainerOutput>)
				}
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

	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		_Module.ContextMenu(wParam, lParam);
/*
		if((HWND)wParam == m_view.m_hWnd)
		{
			CMenu menuContext;
			menuContext.LoadMenu(IDR_CONTEXTMENU);
			CMenuHandle menuPopup(menuContext.GetSubMenu(0));
#ifndef _WIN32_WCE
			m_CmdBar.TrackPopupMenu(menuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, LOWORD(lParam), HIWORD(lParam));
#else
			TrackPopupMenuEx(menuPopup, TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), m_hWndCECommandBar, NULL);
#endif // _WIN32_WCE
		}
		else
		{
			bHandled = FALSE;
		}
*/
		return 0;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;//1;   // no background needed
	}
// Custom draw overrides
//	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
/*
	{
		return CDRF_NOTIFYITEMDRAW;   // we need per-item notifications//CDRF_DODEFAULT;
	}
*/
/*
	// drawing helper - draws "x" button image
	void DrawButtonImage(CDCHandle dc, RECT& rcImage, HPEN hPen)
	{
#if !defined(_WIN32_WCE) || (_WIN32_WCE >= 400)
		HPEN hPenOld = dc.SelectPen(hPen);

		dc.MoveTo(rcImage.left, rcImage.top);
		dc.LineTo(rcImage.right, rcImage.bottom);
		dc.MoveTo(rcImage.left + 1, rcImage.top);
		dc.LineTo(rcImage.right + 1, rcImage.bottom);

		dc.MoveTo(rcImage.left, rcImage.bottom - 1);
		dc.LineTo(rcImage.right, rcImage.top - 1);
		dc.MoveTo(rcImage.left + 1, rcImage.bottom - 1);
		dc.LineTo(rcImage.right + 1, rcImage.top - 1);

		dc.SelectPen(hPenOld);
#else // (_WIN32_WCE < 400)
		rcImage;
		hPen;
		// no support for the "x" button image
#endif // (_WIN32_WCE < 400)
	}
*/
/*
	DWORD OnItemPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		//Get button text
		TBBUTTON tButton;
		ATLVERIFY(m_tb.GetButton(idCtrl, &tButton));
		CString strButtonText;
		int cchMaxLen = 256;
		ATLVERIFY(m_tb.GetString(tButton.iString, strButtonText.GetBuffer(cchMaxLen), cchMaxLen) != -1);
		strButtonText.ReleaseBuffer();

		CDCHandle dc = lpNMCustomDraw->hdc;
#if (_WIN32_IE >= 0x0400)
		RECT& rc = lpNMCustomDraw->rc;
#else // !(_WIN32_IE >= 0x0400)
		RECT rc = { 0 };
		m_tb.GetItemRect(0, &rc);
#endif // !(_WIN32_IE >= 0x0400)

		RECT rcImage = { m_xBtnImageLeft, m_yBtnImageTop, m_xBtnImageRight + 1, m_yBtnImageBottom + 1 };
		::OffsetRect(&rcImage, rc.left, rc.top);
//		T* pT = static_cast<T*>(this);

		if((lpNMCustomDraw->uItemState & CDIS_DISABLED) != 0)
		{
			RECT rcShadow = rcImage;
			::OffsetRect(&rcShadow, 1, 1);
			CPen pen1;
			pen1.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_3DHILIGHT));
			DrawButtonImage(dc, rcShadow, pen1);
			CPen pen2;
			pen2.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW));
			DrawButtonImage(dc, rcImage, pen2);
		}
		else
		{
			if((lpNMCustomDraw->uItemState & CDIS_SELECTED) != 0)
				::OffsetRect(&rcImage, 1, 1);
			CPen pen;
			pen.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_BTNTEXT));
			DrawButtonImage(dc, rcImage, pen);
		}

		return CDRF_DODEFAULT;   // continue with the default item painting
	}
*/
protected:
private:
	CToolBarCtrlListen m_ctlToolBarListen;
//	HWND m_hWndToolBarListen;
//	CDlgPortScanner m_dlgPortScanner;
//	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
/*
	{
		bHandled = FALSE;
		return 0;
	}
*/
//	LRESULT OnNotify(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
};
