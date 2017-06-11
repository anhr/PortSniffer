// PageOptionsHTML.h: interface for the CPageOptionsHTML class.
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MyPropertyPageImpl.h"
#include "ContextualHelp\ContextualHelp.h"
#include "MyEdit.h"

class CPageOptionsHTML;
typedef CMyPropertyPageImpl<CPageOptionsHTML> CPageOptionsHTMLImpl;

class CPageOptionsHTML
	: public CPageOptionsHTMLImpl
	, public CWinDataExchange<CPageOptionsHTML>//for DDX
	, private CContextualHelp
{
public:
	CPageOptionsHTML()
		: CPageOptionsHTMLImpl(IDS_GENERAL)//General
	{
	}
	enum { IDD = IDD_PAGE_OPTIONS_HTML };

	BOOL OK()
	{
		ATLTRACE(_T("CPageOptionsHTML::OK()\n"));
		if(!::IsWindow(m_hWnd))
			return TRUE;

		DoDataExchange(true);

		CRegKeyPortScanner regKeyPortScanner;

		regKeyPortScanner.SetApplacationDataFolder(m_editFolder.GetWindowText());

		switch(IsDlgButtonChecked(IDC_CHC_REPLACE_HOST))
		{
		case BST_CHECKED:// The button is checked.
			regKeyPortScanner.SetReplaceHostHeader(true);
			break;
		case BST_UNCHECKED:// The button is not checked.
			regKeyPortScanner.SetReplaceHostHeader(false);
			break;
		case BST_INDETERMINATE:// The button is in an indeterminate state (applies only if the button has the BS_3STATE or BS_AUTO3STATE style).
		default: ATLASSERT(FALSE);
		}

		switch(IsDlgButtonChecked(IDC_CHC_SAVE_TO_FILE))
		{
		case BST_CHECKED:// The button is checked.
			regKeyPortScanner.SetSaveResponse(true);
			break;
		case BST_UNCHECKED:// The button is not checked.
			regKeyPortScanner.SetSaveResponse(false);
			break;
		case BST_INDETERMINATE:// The button is in an indeterminate state (applies only if the button has the BS_3STATE or BS_AUTO3STATE style).
		default: ATLASSERT(FALSE);
		}

		return TRUE;
	}

	BEGIN_MSG_MAP(CPageOptionsHTML)
		{
			CString strError;
			try
			{
				MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
				NOTIFY_CODE_HANDLER(PSN_HELP, OnHelp2)
				CHAIN_MSG_MAP(CContextualHelp)
				CHAIN_MSG_MAP(CPageOptionsHTMLImpl)
				COMMAND_HANDLER(IDC_CHC_SAVE_TO_FILE, BN_CLICKED, OnBnClickedChcSaveToFile)
				COMMAND_HANDLER(IDC_BTN_BROWSE, BN_CLICKED, OnBnClickedBtnBrowse)
			}
			catch(CException* e)
			{
				strError = e->GetErrorMessage();
				delete e;
			}
			catch(...)
			{
				strError = _T("Unhandled exception!!!");
			}
			if(!strError.IsEmpty())
			{
				::EmailMe(_T("ERROR: CPageOptionsHTML message map failed! ") + strError, NULL, m_hWnd);
				return 1;
			}
		}
	END_MSG_MAP()

  BEGIN_DDX_MAP(CPageOptionsHTML)
		DDX_CONTROL(IDC_EDIT_RESPONSE_FOLDER, m_editFolder)
  END_DDX_MAP()

private:
	CMyEdit m_editFolder;

	void Enable()
	{
		if(!::IsWindow(m_hWnd))
			return;

		bool boSaveToFile = false;
		switch(IsDlgButtonChecked(IDC_CHC_SAVE_TO_FILE))
		{
		case BST_CHECKED:// The button is checked.
			boSaveToFile = true;
			break;
		case BST_UNCHECKED:// The button is not checked.
			boSaveToFile = false;
			break;
		case BST_INDETERMINATE:// The button is in an indeterminate state (applies only if the button has the BS_3STATE or BS_AUTO3STATE style).
		default: ATLASSERT(FALSE);
		}

		::EnableWindow(GetDlgItem(IDC_EDIT_RESPONSE_FOLDER), boSaveToFile);
		::EnableWindow(GetDlgItem(IDC_BTN_BROWSE), boSaveToFile);
	}


	void Help()
	{
		_Module.HelpFinder(_T("afx_hidd_options_html.htm"));
	}

	LRESULT OnHelp2(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		Help();
		return 1;
	}
	virtual CString GetText(int iCtrlId)
	{
		CString strText;
		UINT nID;
		switch(iCtrlId)
		{
		case IDC_CHC_REPLACE_HOST: nID = IDS_CHC_REPLACE_HOST;//Replaces the "Host: localhost" line to "Host: <URL>"
			break;
		case IDC_CHC_SAVE_TO_FILE: nID = IDS_CHC_SAVE_TO_FILE;//Save the server's response to a file
			break;
		case IDC_EDIT_RESPONSE_FOLDER: nID = IDS_EDIT_RESPONSE_FOLDER;//Folder for saved files
			break;
		case IDC_BTN_BROWSE: nID = IDS_BTN_BROWSE;//Opens the folder dialog where you can choose a folder to save server's responses.
			break;
		default: return CContextualHelp::GetText(iCtrlId);
		}
		strText.LoadString(nID);
		return strText;
	}
/*
	void Update()
	{
		Enable();
	}
*/
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
    // Hook up controls & variables
    DoDataExchange(false);

		CRegKeyPortScanner regKeyPortScanner;

		CheckDlgButton(IDC_CHC_REPLACE_HOST, regKeyPortScanner.IsReplaceHostHeader() ? BST_CHECKED: BST_UNCHECKED);
		CheckDlgButton(IDC_CHC_SAVE_TO_FILE, regKeyPortScanner.IsSaveResponse() ? BST_CHECKED: BST_UNCHECKED);

		m_editFolder.SetWindowText(regKeyPortScanner.GetApplacationDataFolder());

		Enable();
		return 0;
	}

	LRESULT OnBnClickedChcSaveToFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		Enable();
		return 0;
	}
	LRESULT OnBnClickedBtnBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString strTitle;
		strTitle.LoadString(IDS_SELECT_RESPONSE_FOLDER);//"Please select a folder where server's responses should be saved:"
		WTL::CFolderDialog dlg(m_hWnd, strTitle, BIF_RETURNONLYFSDIRS | BIF_USENEWUI);//BIF_EDITBOX | BIF_NEWDIALOGSTYLE);

		//ATTENTION!!! I do not see the initial folder if I use
		//dlg.SetInitialFolder(m_editFolder.GetWindowText());
		//line
		CString strFolder = m_editFolder.GetWindowText();
		dlg.SetInitialFolder(strFolder);

		INT_PTR nRet = dlg.DoModal();
		if (nRet == IDOK)
			m_editFolder.SetWindowText(dlg.GetFolderPath());
		return 0;
	}
};
