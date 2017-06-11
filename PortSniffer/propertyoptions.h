// PropertyOptions.h: interface for the CPropertyOptions class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "PageOptionsHTML.h"

class CPropertyOptions
	: public CPropertySheetImpl<CPropertyOptions>
{
public:
	enum Pages
	{
		HTML
	};

	CPropertyOptions(CPropertyOptions::Pages activePage = CPropertyOptions::HTML)
		: m_activePage(activePage)
		, m_pageOptionsHTML()
		, m_boError(false)
//		, m_boFinish(false)
	{

		m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_USEHICON;// | PSH_NOCONTEXTHELP;//PSH_USEICONID;//;// | PSH_HASHELP;
		m_psh.hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

		EnableHelp();

		ATLVERIFY(AddPage(m_pageOptionsHTML));
		ATLVERIFY(SetActivePage(m_activePage));

		CString strMainFrame;
		ATLVERIFY(strMainFrame.LoadString(IDR_MAINFRAME));//Port Scanner
		ATLVERIFY(m_strTitle.LoadString(IDS_OPTIONS));//Options
		m_strTitle = strMainFrame + _T(" ") + m_strTitle;
		SetTitle(m_strTitle);
	}

	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		INT_PTR res = CPropertySheetImpl<CPropertyOptions>::DoModal(hWndParent);
		if(m_boError)
			return 0;//Cancel
		return res;
	}
	BEGIN_MSG_MAP(CPropertyOptions)
		{
			CString strError;
			try
			{
				COMMAND_ID_HANDLER(ID_SET_WIZARD_BUTTONS, OnSetWizardButtons)
				COMMAND_ID_HANDLER(IDOK, OnOK)
//				MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
//				COMMAND_ID_HANDLER(ID_INITIALIZE, OnInitialize)
				NOTIFY_CODE_HANDLER(PSN_WIZFINISH, OnFinish)
				CHAIN_MSG_MAP_MEMBER(m_pageOptionsHTML)
			}
/*
			catch (_com_error& e)
			{
				HRESULT hr = e.Error();
				CString strResult;
				strResult.Format(_T("0x%x"), hr);
				CString strDescription;
				if(e.ErrorInfo())
					strDescription = (char*)e.Description();
				strError = _T("HRESULT message is: \"") + CString(e.ErrorMessage()) + _T("\" (") + strResult + _T(")") + strDescription;
			}
*/
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
				ATLASSERT(!m_boError);
				m_boError = true;
				strError = _T("ERROR: CPropertyOptions message map failed! ") + strError;
				::EmailMe(strError, NULL, m_hWnd);
			}
		}
	END_MSG_MAP()

	void OnSheetInitialized()
	{
//		PostMessage(WM_COMMAND, ID_INITIALIZE, 0);
	}

private:
	CString m_strTitle;
	CPageOptionsHTML m_pageOptionsHTML;
	Pages m_activePage;
	bool m_boError;
	LRESULT OnSetWizardButtons(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		return 1;
	}

	BOOL Finish()
	{
		ATLTRACE(_T("CPropertyOptions::Finish()\n"));
		if(!m_pageOptionsHTML.OK())
		{
			SetActivePage(HTML);
			return TRUE;
		}
		return FALSE;//Finish was completed. Close dialog
	}

	LRESULT OnFinish(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		return Finish();
	}
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		if(Finish())
			return 0;
		bHandled = FALSE;
		return 1;
	}
};
