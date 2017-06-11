// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "PaneContainerOutput.h"
#include "PaneContainerBanner.h"
#include <atlframe.h>//for CFrameWindowImpl
#include <atlsplit.h>//for CHorSplitterWindow
#include "PortSnifferView.h"
#include <atlctrlw.h>//for CCommandBarCtrl
#include "AboutDlg.h"
#include <atlprint.h>//for printer
#include "Version.h"
#include "OSVersion.h"
#include "BannerView.h"
#include "MyStatusBarCtrl.h"
#include "SplitterPortSniffer.h"

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
#ifndef _WIN32_WCE
	, public CPrintJobInfo
#endif // _WIN32_WCE
{
public:
	CMainFrame()
//		: m_ctlToolBarListen(this)
//		, m_ctlToolBarBanner(this)
		: m_bModified(FALSE)
		, m_rcMargin(1000, 1000, 1000, 1000)
	{
#ifndef _WIN32_WCE
		m_printer.OpenDefaultPrinter();
		m_devmode.CopyFromPrinter(m_printer);
#endif // _WIN32_WCE
	}

	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		BOOL bModified = m_view.GetModify();
		if(bModified != m_bModified)
		{
			m_bModified = bModified;
			SetTitle();
//			ATLVERIFY(SetWindowText(UpdateTitle()));
		}

		ATLVERIFY(CUpdateUIBase::UIEnable(ID_EDIT_COPY, (BOOL)m_view.CanEdit()));

#ifndef _WIN32_WCE
		if(m_prev.IsWindow() && (m_prev.GetStyle() & WS_VISIBLE) != 0)
		{
			UIEnable(ID_PP_BACK, (m_prev.m_nCurPage > m_prev.m_nMinPage));
			UIEnable(ID_PP_FORWARD, m_prev.m_nCurPage < m_prev.m_nMaxPage);
		}
#endif // _WIN32_WCE

		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_EDIT_COPY, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_LISTEN, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_SELECT_ALL, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_PP_BACK,	  UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_PP_FORWARD, UPDUI_TOOLBAR)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		{
			CString strError;
			try
			{
//COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
//COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
				MESSAGE_HANDLER(WM_CREATE, OnCreate)
				MESSAGE_HANDLER(WM_CLOSE, OnClose)
				MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
//				COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
				COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
				COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
				COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
				COMMAND_ID_HANDLER(ID_TOOLS_OPTIONS, OnToolsOptions)
				COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)

				//help
				COMMAND_ID_HANDLER(ID_HELP_FINDER, OnHelpFinder)
				COMMAND_ID_HANDLER(ID_CONTEXT_HELP, OnHelpFinder)
				MESSAGE_HANDLER(WMU_HELP_FINDER, OnHelpFinder2)

#ifndef _WIN32_WCE
				//print
				COMMAND_ID_HANDLER(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
				COMMAND_ID_HANDLER(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
				COMMAND_ID_HANDLER(ID_PP_CLOSE, OnPrintPreviewClose)
				COMMAND_ID_HANDLER(ID_PP_BACK, OnPrintPreviewBack)
				COMMAND_ID_HANDLER(ID_PP_FORWARD, OnPrintPreviewForward)
				COMMAND_ID_HANDLER(ID_FILE_PRINT, OnFilePrint)
#endif // _WIN32_WCE

				CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
				CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
				MESSAGE_HANDLER(WMU_STATUS, OnStatus)
				MESSAGE_HANDLER(WMU_MENU, OnMenu)
				MESSAGE_HANDLER(WMU_UPDATE_TITLE, OnUpdateTitle)
//				MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
				MESSAGE_HANDLER(WMU_CONTEXT_MENU, OnContextMenu)
				CHAIN_MSG_MAP_MEMBER(m_view)

				//ATTENTION!!! Include lines below after CHAIN_MSG_MAP_MEMBER(m_view) line only
				COMMAND_ID_HANDLER(ID_LISTEN, OnListen)

				//ATTENTION!!! Include line below before CHAIN_MSG_MAP_MEMBER(m_ctlToolBarListen) line only
				MESSAGE_HANDLER(CMyToolBarCtrl::GetEnableMsg(), OnEnable)

				CHAIN_MSG_MAP_MEMBER(m_statusBar)

				if
					(
						(uMsg == CMyToolBarCtrl::GetEnableMsg())
						|| ((uMsg > WMU_FIRST) && (uMsg < WMU_LAST))
						|| (uMsg == WM_COMMAND && ((LOWORD(wParam) == ID_EDIT_CUT) || (LOWORD(wParam) == ID_EDIT_COPY) || (LOWORD(wParam) == ID_EDIT_PASTE)))
					)
				{
					CHAIN_MSG_MAP_MEMBER(m_paneOutput)
				}

//				CHAIN_MSG_MAP_MEMBER(m_paneBanner)
				CHAIN_MSG_MAP_MEMBER(m_bannerView)

/*
				//ATTENTION!!! Include line below before CHAIN_MSG_MAP_MEMBER(m_ctlToolBarListen) line only for WM_DESTROY
				CHAIN_MSG_MAP_MEMBER(m_ctlToolBarBanner)

				CHAIN_MSG_MAP_MEMBER(m_ctlToolBarListen)
*/
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

private:
	CSplitterPortSniffer m_splitter;
	CPaneContainerBanner m_paneBanner;
	CPaneContainerOutput m_paneOutput;
	CBannerView m_bannerView;

	CPortSnifferView m_view;
	CCommandBarCtrl m_CmdBar;
//	CToolBarCtrlListen m_ctlToolBarListen;
//	CToolBarCtrlBanner m_ctlToolBarBanner;
	BOOL m_bModified;
	CMyStatusBarCtrl m_statusBar;

	//print
#ifndef _WIN32_WCE
	CPrinterT<true> m_printer;
	CDevModeT<true> m_devmode;
	CPrintPreviewWindow m_prev;
//	CEnhMetaFileT<true> enhmetafile;
#endif // _WIN32_WCE
	CRect m_rcMargin;
	HWND m_hWndOldClient;
	HWND m_hWndToolBarPP;
	CSimpleValArray<long> m_arrPages;

#ifndef _WIN32_WCE
	//print job info callback
	virtual bool IsValidPage(UINT /*nPage*/)
	{
		return true;
	}

	virtual bool PrintPage(UINT nPage, HDC hDC)
	{
		if (nPage >= (UINT)m_arrPages.GetSize())
			return false;

		RECT rcPage;
		rcPage.left = rcPage.top = 0;
		rcPage.bottom = GetDeviceCaps(hDC, PHYSICALHEIGHT);
		rcPage.right = GetDeviceCaps(hDC, PHYSICALWIDTH);

		rcPage.right = MulDiv(rcPage.right, 1440, GetDeviceCaps(hDC, LOGPIXELSX));
		rcPage.bottom = MulDiv(rcPage.bottom, 1440, GetDeviceCaps(hDC, LOGPIXELSY));

		RECT rcOutput = rcPage;
		//convert from 1/1000" to twips
		rcOutput.left += MulDiv(m_rcMargin.left, 1440, 1000);
		rcOutput.right -= MulDiv(m_rcMargin.right, 1440, 1000);
		rcOutput.top += MulDiv(m_rcMargin.top, 1440, 1000);
		rcOutput.bottom -= MulDiv(m_rcMargin.bottom, 1440, 1000);

		
		FORMATRANGE fr;
		fr.hdc = hDC;
		fr.hdcTarget = hDC;
		fr.rc = rcOutput;
		fr.rcPage = rcPage;
		fr.chrg.cpMin = 0;
		fr.chrg.cpMax = -1;
		fr.chrg.cpMin = m_arrPages[nPage];

		// We have to adjust the origin because 0,0 is not at the corner of the paper
		// but is at the corner of the printable region
		int nOffsetX = GetDeviceCaps(hDC, PHYSICALOFFSETX);
		int nOffsetY = GetDeviceCaps(hDC, PHYSICALOFFSETY);
		SetViewportOrgEx(hDC, -nOffsetX, -nOffsetY, NULL);
		m_view.FormatRange(fr, TRUE);
		m_view.DisplayBand(&rcOutput);

		//Cleanup cache in richedit
		m_view.FormatRange(NULL, FALSE);
		return true;
	}

	bool LayoutPages()
	{
		CDC dc = m_printer.CreatePrinterDC(m_devmode);
		if(dc.IsNull())
			return false;

		RECT rcPage;
		rcPage.left = rcPage.top = 0;
		rcPage.bottom = GetDeviceCaps(dc, PHYSICALHEIGHT);
		rcPage.right = GetDeviceCaps(dc, PHYSICALWIDTH);
		// We have to adjust the origin because 0,0 is not at the corner of the paper
		// but is at the corner of the printable region
		int nOffsetX = dc.GetDeviceCaps(PHYSICALOFFSETX);
		int nOffsetY = dc.GetDeviceCaps(PHYSICALOFFSETY);
		dc.SetViewportOrg(-nOffsetX, -nOffsetY);
		rcPage.right = MulDiv(rcPage.right, 1440, GetDeviceCaps(dc, LOGPIXELSX));
		rcPage.bottom = MulDiv(rcPage.bottom, 1440, GetDeviceCaps(dc, LOGPIXELSY));

		RECT rcOutput = rcPage;
		//convert from 1/1000" to twips
		rcOutput.left += MulDiv(m_rcMargin.left, 1440, 1000);
		rcOutput.right -= MulDiv(m_rcMargin.right, 1440, 1000);
		rcOutput.top += MulDiv(m_rcMargin.top, 1440, 1000);
		rcOutput.bottom -= MulDiv(m_rcMargin.bottom, 1440, 1000);
		
		FORMATRANGE fr;
		fr.hdc = dc;
		fr.hdcTarget = dc;
		fr.rc = rcOutput;
		fr.rcPage = rcPage;
		fr.chrg.cpMin = 0;
		fr.chrg.cpMax = -1;

		LONG n = (LONG)m_view.GetTextLength();
		m_arrPages.RemoveAll();
		while (1)
		{
			m_arrPages.Add(fr.chrg.cpMin);
			LONG lRet = m_view.FormatRange(fr, FALSE);
			if((lRet - fr.chrg.cpMin) == -1)
			{
				m_arrPages.RemoveAt(m_arrPages.GetSize()-1);
				break;
			}
			else
				fr.chrg.cpMin = lRet;
			if (fr.chrg.cpMin >= n)
				break;
		}

		m_view.FormatRange(NULL, FALSE);

		return true;
	}
#endif // _WIN32_WCE

	void SetTitle()
	{ ATLVERIFY(SetWindowText(UpdateTitle()));}

	CString UpdateTitle(LPCTSTR szWindowName = NULL)
	{
		CString strMainFrame;
		if(!szWindowName)
		{
			strMainFrame.LoadString(IDR_MAINFRAME);
			szWindowName = strMainFrame;
		}
		return CString(m_view.GetFileName()) + (m_bModified ? _T("*") : _T("")) + _T(" - ") + szWindowName;
	}

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;//Allow the m_ctlToolBarListen enabling
		ATLVERIFY(CUpdateUIBase::UIEnable((UINT)wParam, (BOOL)lParam));
		return TRUE;
	}

LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
//LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
/*
{
	bHandled = FALSE;
	return 0;
}
*/
//LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
/*
{
	bHandled = FALSE;
	return 0;
}
*/
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		_Module.SetMainFrame(this);

		// create command bar window
		HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		// attach menu
		m_CmdBar.AttachMenu(GetMenu());
		// load command bar images
		m_CmdBar.LoadImages(IDR_MAINFRAME);
		// remove old menu
		SetMenu(NULL);

		HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

		CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		AddSimpleReBarBand(hWndCmdBar);
		AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

		//Status bar
		CreateSimpleStatusBar(_T(""));
		m_statusBar = m_hWndStatusBar;
		const int nParts = 2;
		int nWidths[nParts];
		nWidths[0] = 150;
		nWidths[1] = -1;
		m_statusBar.SetParts(nParts, nWidths);
		CString strReady;
		strReady.LoadString(ATL_IDS_IDLEMESSAGE);
		m_statusBar.SetText(1, strReady);


		m_hWndClient = m_splitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_paneBanner.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);// | PANECNT_NOBORDER);
//		m_pane.Create(m_splitter, _T("Tree"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		m_paneBanner.Create(m_splitter);
/*
		m_treeview.Create(m_paneBanner, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
		m_treeview.SetFont(AtlGetDefaultGuiFont());
*/
		m_bannerView.Create(m_paneBanner);
		m_paneBanner.AdviseSinkMap();
/*
		CString strAppName;
		strAppName.LoadString(IDR_MAINFRAME);//Port Sniffer
		m_bannerView.Create(m_paneBanner, rcDefault
			, _T("http://")
#ifdef DEBUG
				_T("localhost")
#else //DEBUG
				_T("bonalink.com")
#endif //DEBUG
				_T("/banner.srf?object=") + strAppName
			, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);
		CComPtr<IUnknown> spUnk;
		::AtlAxGetControl(m_bannerView.m_hWnd,&spUnk);
		ATLASSERT(spUnk);
		hRet = spUnk->QueryInterface(__uuidof(IWebBrowser2), (void**)&m_pBrowserApp);
		if(FAILED(hRet))
			throw new CException(_T("AtlAxGetControl failed! ") + ::GetLastErrorString(hRet));
		ATLASSERT(m_pBrowserApp);
*/
		m_paneBanner.SetClient(m_bannerView);

		m_paneOutput.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);// | PANECNT_NOBORDER);
//		m_paneOutput.Create(m_splitter, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);// | WS_BORDER);
		m_paneOutput.Create(m_splitter);
//		m_view.Create(m_splitter, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_SAVESEL, WS_EX_CLIENTEDGE);
		m_view.Create(m_paneOutput, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_SAVESEL, WS_EX_CLIENTEDGE);
		m_view.SetFont(AtlGetDefaultGuiFont());
		m_paneOutput.SetClient(m_view);

		m_splitter.SetSplitterPanes(m_paneBanner, m_paneOutput);//m_view);
		UpdateLayout();
		m_splitter.SetSplitterPosDefault(m_paneBanner.m_cxyHeader);
//		m_splitter.SetSplitterPosPct(25);
/*
		m_splitter.SetSplitterPos(90//banner
			+ 12//border
			* 2);
*/
		UIAddToolBar(hWndToolBar);
		UISetCheck(ID_VIEW_TOOLBAR, 1);
		UISetCheck(ID_VIEW_STATUS_BAR, 1);
		UISetCheck(ID_VIEW_TREEPANE, 1);

#ifndef _WIN32_WCE
		//printer
		m_hWndToolBarPP = CreateSimpleToolBarCtrl(m_hWnd, IDR_PRINTPREVIEWBAR, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE, ATL_IDW_TOOLBAR + 1);
		AddSimpleReBarBand(m_hWndToolBarPP, NULL, TRUE);
		::SendMessage(m_hWndToolBar, RB_SHOWBAND, 2, FALSE);	// print preview toolbar is band #2
		UIAddToolBar(m_hWndToolBarPP);
#endif // _WIN32_WCE

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = !m_view.QueryClose();
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
//		_Module.Destroy();
		CMyStatusBarCtrl::Destroy();

		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString strAppName;
		strAppName.LoadString(IDR_MAINFRAME);//Port Sniffer
		CAboutDlg dlg(strAppName, ::GetModuleVersion2(_Module.GetModuleInstance()) + ::OSVersion());
		dlg.DoModal();
		return 0;
	}

	LRESULT OnMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
//		bHandled = FALSE;
		
		*((HMENU*)wParam) = m_CmdBar.GetMenu();
		return TRUE;
	}

	LRESULT OnUpdateTitle(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
//		bHandled = FALSE;
		
		SetTitle();
		return TRUE;
	}

	void Status(LPCTSTR lpszStatus)//, int nPart = 1)
	{
//		CMyStatusBarCtrl sb = m_hWndStatusBar;
//		sb.SetText(1, lpszStatus);
		m_statusBar.SetText(1, lpszStatus);
	}

/*
	void Status(LPCTSTR lpszStatus, int nPart = 1)
	{
		if(!m_hWndStatusBar || !::IsWindow(m_hWndStatusBar))
			return;

		CString strStatus = lpszStatus;
		if(strStatus.GetLength() > 250)
		{
			ATLASSERT(FALSE);
			strStatus = strStatus.Left(250);
		}
//		::SendMessage(m_hWndStatusBar, SB_SETTEXT, 0, (LPARAM)(LPCTSTR)strStatus);
//		CMyStatusBarCtrl sb = m_hWndStatusBar;
//		sb.SetText(nPart, strStatus);
		m_statusBar.SetText(nPart, strStatus);
	}

	void Status(CMyStatusBarCtrl::CStatus* pstatus)
	{
		Status(pstatus->GetText(), pstatus->GetPart());
		delete pstatus;
	}
*/
	LRESULT OnStatus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
//		bHandled = FALSE;
//		Status((CMyStatusBarCtrl::CStatus*)wParam);
/*
		CMyStatusBarCtrl sb = m_hWndStatusBar;
		sb.Status((CMyStatusBarCtrl::CStatus*)wParam);
*/
		m_statusBar.Status((CMyStatusBarCtrl::CStatus*)wParam);
		return TRUE;
	}

	LRESULT OnListen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
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

		return 1;
	}
	LRESULT OnPrintPreviewClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_prev.DestroyWindow();
		m_hWndClient = m_hWndOldClient;
		::ShowWindow(m_hWndClient, SW_SHOW);
		::SendMessage(m_hWndToolBar, RB_SHOWBAND, 1, TRUE); // toolbar is band #1
		::SendMessage(m_hWndToolBar, RB_SHOWBAND, 2, FALSE); // print preview toolbar is band #4
/*
		::SendMessage(m_hWndToolBar, RB_SHOWBAND, 2, TRUE); // Listen toolbar is band #2
		::SendMessage(m_hWndToolBar, RB_SHOWBAND, 3, TRUE); // Banner toolbar is band #3
		::SendMessage(m_hWndToolBar, RB_SHOWBAND, 4, FALSE); // print preview toolbar is band #4
*/
		UpdateLayout();
		return 0;
	}
	void StatusPrint()
	{
		CString strStatus;
		strStatus.Format(IDS_STATUS_PRINT, m_prev.m_nCurPage + 1, m_prev.m_nMaxPage + 1);//Page %d of %d.
		Status(strStatus);
	}
	LRESULT OnPrintPreviewForward(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_prev.NextPage();
		StatusPrint();
		return 0;
	}
	
	LRESULT OnPrintPreviewBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_prev.PrevPage();
		StatusPrint();
		return 0;
	}

	LRESULT OnFilePrint(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(!LayoutPages())
		{
			MessageBox(_T("Print operation failed"), _T("MTPad"), MB_ICONERROR | MB_OK);
			return 0;
		}

		CPrintDialog dlg(FALSE);
		dlg.m_pd.hDevMode = m_devmode.CopyToHDEVMODE();
		dlg.m_pd.hDevNames = m_printer.CopyToHDEVNAMES();
		dlg.m_pd.nMinPage = 1;
		dlg.m_pd.nMaxPage = (WORD)m_arrPages.GetSize();
		dlg.m_pd.nFromPage = 1;
		dlg.m_pd.nToPage = (WORD)m_arrPages.GetSize();
		dlg.m_pd.Flags &= ~PD_NOPAGENUMS;

		if (dlg.DoModal() == IDOK)
		{
			m_devmode.CopyFromHDEVMODE(dlg.m_pd.hDevMode);
			m_printer.ClosePrinter();
			m_printer.OpenPrinter(dlg.m_pd.hDevNames, m_devmode.m_pDevMode);

			CPrintJob job;
			int nMin=0;
			int nMax = m_arrPages.GetSize() - 1;
			if(dlg.PrintRange() != FALSE)
			{
				nMin = dlg.m_pd.nFromPage - 1;
				nMax = dlg.m_pd.nToPage - 1;
			}

			job.StartPrintJob(false, m_printer, m_devmode.m_pDevMode, this, _T("MTPad Document"), nMin, nMax, (dlg.PrintToFile() != FALSE));
		}

		GlobalFree(dlg.m_pd.hDevMode);
		GlobalFree(dlg.m_pd.hDevNames);

		return 0;
	}

	LRESULT OnFilePrintSetup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFilePrintPreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnHelpFinder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnHelpFinder2(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void HelpFinder(LPCTSTR lpszTopic = NULL);

	LRESULT OnToolsOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
