//mainfrm.CPP, implementation of the CMainFrame class.

#include "StdAfx.h"
#include "MainFrm.h"
#include <HtmlHelp.h>
#include "FileManagement.h"//for GetFilePath(...)
#include "PropertyOptions.h"

//////////////////////////////////////////////////////////////////////
// Enable debug memory manager
#ifdef _DEBUG
#ifdef THIS_FILE
#undef THIS_FILE
#endif //THIS_FILE
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

LRESULT CMainFrame::OnListen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
//	bHandled = FALSE;

	bool boListening = m_view.IsListening();

	UISetCheck(ID_LISTEN, boListening);
	UpdateLayout();

	m_paneOutput.SetButtonState(ID_LISTEN, boListening);

	return 1;
}

LRESULT CMainFrame::OnFilePrintSetup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CPageSetupDialog dlg;
	dlg.m_psd.hDevMode = m_devmode.CopyToHDEVMODE();
	dlg.m_psd.hDevNames = m_printer.CopyToHDEVNAMES();
	dlg.m_psd.rtMargin = m_rcMargin;

	if (dlg.DoModal() == IDOK)
	{
		m_devmode.CopyFromHDEVMODE(dlg.m_psd.hDevMode);
		m_printer.ClosePrinter();
		m_printer.OpenPrinter(dlg.m_psd.hDevNames, m_devmode.m_pDevMode);
		m_rcMargin = dlg.m_psd.rtMargin;
	}

	GlobalFree(dlg.m_psd.hDevMode);
	GlobalFree(dlg.m_psd.hDevNames);

	return 0;
}

LRESULT CMainFrame::OnFilePrintPreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool bRet = LayoutPages();
	if(!bRet)
	{
		MessageBox(_T("Print preview operation failed"), _T("MTPad"), MB_ICONERROR | MB_OK);
		return 0;
	}

	m_prev.SetPrintPreviewInfo(m_printer, m_devmode.m_pDevMode, this, 0, m_arrPages.GetSize() - 1);
	m_prev.SetPage(0);
	RECT rcPos;
	GetClientRect(&rcPos);
	m_prev.Create(m_hWnd, rcPos);
	m_prev.ShowWindow(SW_SHOW);
	m_prev.SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);

	::ShowWindow(m_hWndClient, SW_HIDE);
	m_hWndOldClient = m_hWndClient;
	m_hWndClient = m_prev;
	::SendMessage(m_hWndToolBar, RB_SHOWBAND, 1, FALSE);	// toolbar is band #1
	::SendMessage(m_hWndToolBar, RB_SHOWBAND, 2, TRUE); // print preview toolbar is band #4
/*
	::SendMessage(m_hWndToolBar, RB_SHOWBAND, 2, FALSE);	// Listen toolbar is band #2
	::SendMessage(m_hWndToolBar, RB_SHOWBAND, 3, FALSE); // Banner toolbar is band #3
	::SendMessage(m_hWndToolBar, RB_SHOWBAND, 4, TRUE); // print preview toolbar is band #4
*/
	UpdateLayout();
	StatusPrint();
	return 0;
}

void CMainFrame::HelpFinder(LPCTSTR lpszTopic)
{
//	CString strShortName;
	CString strLongPanthName;
//	::GetShortAndLongPathName(strShortName, strLongPanthName, _Module.GetModuleInstance());
	DWORD nLongPanthNameSize = 1024;
	ATLVERIFY(::GetModuleFileName(_Module.GetModuleInstance(), strLongPanthName.GetBuffer(nLongPanthNameSize), nLongPanthNameSize));
	strLongPanthName.ReleaseBuffer();
	CString strModuleFolder = ::GetFilePath(strLongPanthName);
	HWND hWnd = ::HtmlHelp(m_hWnd
//		, strModuleFolder + _T("PortScanner.CHM::/hid_edit_find.htm")//szHelpFilePath "c:\My Documents\MyProjects\PortScanner\Debug\PortScanner.CHM"
		, strModuleFolder + _T("PortSniffer.CHM") + (lpszTopic ? (_T("::/") + CString(lpszTopic)) : _T(""))//szHelpFilePath "c:\My Documents\MyProjects\PortSniffer\Debug\PortSniffer.CHM"
		, 0//nCmd
		, 0//dwData
		);
	ATLASSERT(hWnd && ::IsWindow(hWnd));
}

LRESULT CMainFrame::OnHelpFinder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HelpFinder();
	return 0;
}

LRESULT CMainFrame::OnHelpFinder2(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//		bHandled = FALSE;
	HelpFinder((LPCTSTR)wParam);
	return 1;
}

LRESULT CMainFrame::OnToolsOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CPropertyOptions dlg;
	dlg.DoModal();
	return 0;
}
