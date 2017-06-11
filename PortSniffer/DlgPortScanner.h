// DlgPortScanner.h : interface of the CDlgPortScanner class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AfxMessageBox.h"
#include "resource.h"
#include "Thread.h"
#include "RegKeyPortScanner.h"
#include "Defines.h"//for WMU_OUTPUT
#include <atlctrls.h>//for CComboBox
#include "TransparentCtrls\StaticEx.h"//for CStaticEx
#include <atlddx.h>//for BEGIN_DDX_MAP and CWinDataExchange
#include "MyEdit.h"

class CDlgPortScanner : public CDialogImpl<CDlgPortScanner>
	, public CWinDataExchange<CDlgPortScanner>//for DDX
{
public:
	CDlgPortScanner()
		: m_stcPortLocal(DT_RIGHT)//DT_LEFT | DT_SINGLELINE
		, m_stcPortRemote(DT_RIGHT)//DT_LEFT | DT_SINGLELINE
		, m_stcURL(DT_RIGHT)//DT_LEFT | DT_SINGLELINE
	{}
	enum { IDD = IDD_PORTSCANNER };

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	int//width of the CDlgPortScanner control 
		Create(HWND hWndParent, LPTBBUTTON pTBButtons, int* pnIndex
#ifdef _DEBUG
			, const int nButtonCount
#endif //_DEBUG
		);

	void Enable(bool boListening)// = false)
	{
		::EnableWindow(GetDlgItem(IDC_EDIT_LISTEN_PORT), !boListening);
		::EnableWindow(GetDlgItem(IDC_EDIT_REMOTE_PORT), !boListening);
		::EnableWindow(GetDlgItem(IDC_CMB_URL), !boListening);
	}

	class CItemData
	{
	public:
		CItemData(DWORD dwPortLocal,	DWORD dwPortRemote)
			: m_dwPortLocal(dwPortLocal)
			, m_dwPortRemote(dwPortRemote)
		{}
		DWORD GetPortLocal()
		{ return m_dwPortLocal;}
		DWORD GetPortRemote()
		{ return m_dwPortRemote;}
	private:
		DWORD m_dwPortLocal;
		DWORD m_dwPortRemote;
	};

	BEGIN_MSG_MAP(CDlgPortScanner)
		{
			CString strError;
			try
			{
				MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
//				COMMAND_HANDLER(IDC_CHC_LISTEN, BN_CLICKED, OnBnClickedChcListen)
				COMMAND_HANDLER(IDC_EDIT_LISTEN_PORT, EN_CHANGE, OnEnChangeEditListenPort)
				COMMAND_HANDLER(IDC_EDIT_REMOTE_PORT, EN_CHANGE, OnEnChangeEditListenPort)
				MESSAGE_HANDLER(WMU_URL, OnURL)
				MESSAGE_HANDLER(WMU_PORT_LOCAL, OnPortLocal)
				MESSAGE_HANDLER(WMU_PORT_REMOTE, OnPortRemote)
				COMMAND_HANDLER(IDC_CMB_URL, CBN_EDITCHANGE, OnCbnEditchangeCmbUrl)
				MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
				COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
				COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
				COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
//				CHAIN_MSG_MAP_MEMBER(m_stcPort)
				COMMAND_HANDLER(IDC_CMB_URL, CBN_SELCHANGE, OnCbnSelchangeCmbUrl)
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
				::AfxMessageBox(_T("ERROR: CDlgPortScanner failed! ") + strError, MB_ICONEXCLAMATION, _Module.GetMainFrame()->m_hWnd);
			}
		}
	END_MSG_MAP()

  BEGIN_DDX_MAP(CDlgPortScanner)
		DDX_CONTROL(IDC_EDIT_LISTEN_PORT, m_editPortLocal)
		DDX_CONTROL(IDC_EDIT_REMOTE_PORT, m_editPortRemote)
		DDX_CONTROL(IDC_STC_PORT_LOCAL, m_stcPortLocal)
		DDX_CONTROL(IDC_STC_PORT_REMOTE, m_stcPortRemote)
		DDX_CONTROL(IDC_STC_URL, m_stcURL)
  END_DDX_MAP()
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
private:
	CComboBox m_comboBoxURL;
	CMyEdit m_editPortLocal;
	CMyEdit m_editPortRemote;
	CStaticEx m_stcPortLocal;
	CStaticEx m_stcPortRemote;
	CStaticEx m_stcURL;

	void SetPort();
	void Enable()
	{
		UINT nPortLocal = CDialogImpl<CDlgPortScanner>::GetDlgItemInt(IDC_EDIT_LISTEN_PORT);
		UINT nPortRemote = CDialogImpl<CDlgPortScanner>::GetDlgItemInt(IDC_EDIT_REMOTE_PORT);
		CComBSTR bstrURL;
		ATLVERIFY(CDialogImpl<CDlgPortScanner>::GetDlgItemText(IDC_CMB_URL, bstrURL.m_str));
		bool boURL = bstrURL.Length() == 0 ? false : true;
		bool boEnable = nPortLocal && nPortRemote && boURL;
		_Module.Enable(ID_LISTEN, boEnable);
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
    // Hook up controls & variables
    DoDataExchange(false);

		CRegKeyPortScanner regKeyPortScanner;

		m_comboBoxURL = CDialogImpl<CDlgPortScanner>::GetDlgItem(IDC_CMB_URL);
		ATLASSERT(m_comboBoxURL.m_hWnd && ::IsWindow(m_comboBoxURL.m_hWnd));
		regKeyPortScanner.ComboBoxURLInit(&m_comboBoxURL);
/*
		m_editPortLocal = CDialogImpl<CDlgPortScanner>::GetDlgItem(IDC_EDIT_LISTEN_PORT);
		m_editPortRemote = CDialogImpl<CDlgPortScanner>::GetDlgItem(IDC_EDIT_REMOTE_PORT);
*/
		SetPort();
		return TRUE;
	}

	LRESULT OnEnChangeEditListenPort(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnURL(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
//		bHandled = FALSE;
		CString* pstrURL = (CString*)wParam;
		CComBSTR bstrURL;
		ATLVERIFY(CDialogImpl<CDlgPortScanner>::GetDlgItemText(IDC_CMB_URL, bstrURL.m_str));
		*pstrURL = bstrURL;
		return TRUE;
	}
	LRESULT OnPortLocal(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
//		bHandled = FALSE;
		*((UINT*)wParam) = CDialogImpl<CDlgPortScanner>::GetDlgItemInt(IDC_EDIT_LISTEN_PORT);
		return TRUE;
	}
	LRESULT OnPortRemote(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
//		bHandled = FALSE;
		*((UINT*)wParam) = CDialogImpl<CDlgPortScanner>::GetDlgItemInt(IDC_EDIT_REMOTE_PORT);
		return TRUE;
	}
	LRESULT OnCbnEditchangeCmbUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
//		bHandled = FALSE;

		//Save default host
		int index = m_comboBoxURL.GetCurSel();
		if(index == CB_ERR)
			return 0;
		CRegKeyPortScanner regKeyPortScanner;
		regKeyPortScanner.SetDefaultServerID(index);

		for (index = 0; index < m_comboBoxURL.GetCount(); index++)
			delete (CItemData*)m_comboBoxURL.GetItemData(index);

		return 0;
	}
	LRESULT OnCbnSelchangeCmbUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		HWND hWndFocus = GetFocus();
		if(m_comboBoxURL.IsChild(hWndFocus))
		{
			DWORD dwPosition = m_comboBoxURL.GetEditSel();
			WORD wStart = LOWORD(dwPosition);
			WORD wEnd = HIWORD(dwPosition);
			if(LOWORD(dwPosition) != HIWORD(dwPosition))
			{
				m_comboBoxURL.Copy();
				return 0;
			}
		}
		int nStartChar, nEndChar;
		if(m_editPortLocal.m_hWnd == hWndFocus)
		{
			m_editPortLocal.GetSel(nStartChar, nEndChar);
			if(nStartChar != nEndChar)
			{
				m_editPortLocal.Copy();
				return 0;
			}
		}
		if(m_editPortRemote.m_hWnd == hWndFocus)
		{
			m_editPortRemote.GetSel(nStartChar, nEndChar);
			if(nStartChar != nEndChar)
			{
				m_editPortRemote.Copy();
				return 0;
			}
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		HWND hWndFocus = GetFocus();
		if(m_comboBoxURL.IsChild(hWndFocus))
		{
			DWORD dwPosition = m_comboBoxURL.GetEditSel();
			WORD wStart = LOWORD(dwPosition);
			WORD wEnd = HIWORD(dwPosition);
			if(LOWORD(dwPosition) != HIWORD(dwPosition))
			{
				m_comboBoxURL.Cut();
				return 0;
			}
		}
		int nStartChar, nEndChar;
		if(m_editPortLocal.m_hWnd == hWndFocus)
		{
			m_editPortLocal.GetSel(nStartChar, nEndChar);
			if(nStartChar != nEndChar)
			{
				m_editPortLocal.Cut();
				return 0;
			}
		}
		if(m_editPortRemote.m_hWnd == hWndFocus)
		{
			m_editPortRemote.GetSel(nStartChar, nEndChar);
			if(nStartChar != nEndChar)
			{
				m_editPortRemote.Cut();
				return 0;
			}
		}
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		HWND hWndFocus = GetFocus();
		if(m_comboBoxURL.IsChild(hWndFocus))
		{
			m_comboBoxURL.Paste();
			return 0;
		}
		if(m_editPortLocal.m_hWnd == hWndFocus)
		{
			m_editPortLocal.Paste();
			return 0;
		}
		if(m_editPortRemote.m_hWnd == hWndFocus)
		{
			m_editPortRemote.Paste();
			return 0;
		}
		bHandled = FALSE;
		return 0;
	}
};
