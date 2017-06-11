// PortSnifferView.h : interface of the CPortSnifferView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "AfxMessageBox.h"
#include <atlctrls.h>//for CRichEditCtrl
#include "resource.h"
#include "SocketListeningPortScanner.h"
#include "Defines.h"//for WMU_OUTPUT
#include <atldlgs.h>//for CFileDialog
#include "RegKeyPortScanner.h"

class CPortSnifferView : public CWindowImpl<CPortSnifferView, CRichEditCtrl>
{
public:
	CPortSnifferView()
		: m_boFileNew(true)
	{
		m_strFileName.LoadString(IDS_UNTITLED);//_T("Untitled"))
	}

	DECLARE_WND_SUPERCLASS(NULL, CRichEditCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CPortSnifferView)
		{
			CString strError;
			try
			{
			  MESSAGE_HANDLER(WM_CREATE, OnCreate)
				MESSAGE_HANDLER(WMU_OUTPUT, OnOutput)
				COMMAND_CODE_HANDLER(EN_UPDATE, OnEnUpdateEditOutput)
				COMMAND_ID_HANDLER(ID_LISTEN, OnListen)
				COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
				COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnEditSelectall)
				COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
				COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)
#ifndef _WIN32_WCE
				COMMAND_RANGE_HANDLER(ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, OnFileRecent)
#endif // _WIN32_WCE
				COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
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
				::AfxMessageBox(_T("ERROR: CPortSnifferView failed! ") + strError, MB_ICONEXCLAMATION, _Module.GetMainFrame()->m_hWnd);
			}
		}
	END_MSG_MAP()

	bool IsListening()
	{ return m_socketListening.IsOpen();}
	bool CanEdit()
	{
		LONG nStartChar = 0;
		LONG nEndChar = 0;
		CRichEditCtrl::GetSel(nStartChar, nEndChar);
		return (nStartChar == nEndChar) ? false : true;
	}
	LPCTSTR GetFileName()
	{ return m_strFileName;}
	LPCTSTR GetFilePath()
	{ return m_strFilePath;}

	BOOL QueryClose()
	{
		if(m_socketListening.IsOpen())
		{
			_Module.Listen();//Stop listening
			return FALSE;
		}
		return SaveChanges();
	}

	BOOL SaveChanges()
	{
		if(!GetModify())
			return TRUE;
		CString strMessage;
		strMessage.Format(IDS_SAVE_CHANGES, m_strFileName);//Save changes to %s ?
		switch(::AfxMessageBox(strMessage, MB_YESNOCANCEL | MB_ICONEXCLAMATION))
		{
		case IDCANCEL:
			return FALSE;
		case IDYES:
			if(!m_boFileNew)
				return SaveFile();
			if(!DoFileSaveAs())
				return FALSE;
		}
		return TRUE;
	}
	enum Bold
	{
		bold_no_change//0
		, bold_set//1
		, bold_reset//2
	};
private:
	CSocketListeningPortScanner m_socketListening;
	CString m_strFileName;
	CString m_strFilePath;
	bool m_boFileNew;
	CRecentDocumentList m_mru;
//	CWaitCursor m_w;
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	void Listen(bool boQuitApp = false);

	void Init(LPCTSTR lpstrFilePath, LPCTSTR lpstrFileName, bool boFileNew = false)
	{
		m_strFilePath = lpstrFilePath;
		m_strFileName = lpstrFileName;
		m_boFileNew = boFileNew;
		SetModify(FALSE);
	}

	BOOL LoadFile(LPCTSTR lpstrFilePath)
	{
		_ASSERTE(lpstrFilePath != NULL);

		HANDLE hFile = ::CreateFile(lpstrFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return FALSE;

#ifndef _WIN32_WCE
		EDITSTREAM es;
		es.dwCookie = (DWORD)hFile;
		es.dwError = 0;
		es.pfnCallback = StreamReadCallback;
		CRichEditCtrl::StreamIn(SF_TEXT, es);

		::CloseHandle(hFile);

		return !(BOOL)es.dwError;
#else // _WIN32_WCE
		//TODO - figure out what to do here instead...
		::CloseHandle(hFile);

		return FALSE;
#endif // _WIN32_WCE
	}

	void DoFileNew()
	{
		if(SaveChanges())
		{
			SetWindowText(NULL);
			CString strFileName;
			strFileName.LoadString(IDS_UNTITLED);//_T("Untitled"))
			Init(_T(""), strFileName, true);
			_Module.UpdateTitle();
		}
	}

	BOOL DoFileSaveAs()
	{
		BOOL bRet = FALSE;
		CString strTitle;
		strTitle.LoadString(IDS_SAVE_AS);//Save the port sniffer's log file as
		CString strAllFiles;
		strAllFiles.LoadString(IDS_ALL_FILES);//All Files
		CString strTxtFiles;
		strTxtFiles.LoadString(IDS_TXT_FILES);//Text Files
		CString strFilter = 
			strTxtFiles + _T(" (*.txt)") + (TCHAR) NULL + _T("*.txt") + (TCHAR) NULL//_T("Text Files (*.txt)\0*.txt\0")
			+ strAllFiles + _T(" (*.*)") + (TCHAR) NULL + _T("*.*") + (TCHAR) NULL//_T("All Files (*.*)\0*.*\0")
			;
		WTL::CFileDialog dlg(FALSE, NULL
			, m_strFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
			, strFilter
			);
		dlg.m_ofn.lpstrTitle = strTitle;
		int nRet = dlg.DoModal();

		if(nRet == IDOK)
		{
			ATLTRACE(_T("File path: %s\n"), dlg.m_ofn.lpstrFile);
			bRet = SaveFile(dlg.m_ofn.lpstrFile);
			if(bRet)
				Init(dlg.m_ofn.lpstrFile, dlg.m_ofn.lpstrFileTitle);
		}
		return bRet;
	}

	BOOL SaveFile(LPCTSTR lpstrFilePath = NULL)
	{
		if(!lpstrFilePath)
			lpstrFilePath = m_strFilePath;

		HANDLE hFile = ::CreateFile(lpstrFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
//			MY_EXCEPTION((IDS_CREATE_FILE_FAILED, lpstrFilePath, ::GetLastErrorString()));//Create "%s" file failed! %s
			CString strMessage;
			strMessage.Format(IDS_CREATE_FILE_FAILED, lpstrFilePath, ::GetLastErrorString());//Create "%s" file failed! %s
			::AfxMessageBox(strMessage, MB_ICONSTOP);
			return FALSE;
		}

#ifndef _WIN32_WCE
		EDITSTREAM es;
		es.dwCookie = (DWORD)hFile;
		es.dwError = 0;
		es.pfnCallback = StreamWriteCallback;
		CRichEditCtrl::StreamOut(SF_TEXT, es);

		::CloseHandle(hFile);

		return !(BOOL)es.dwError;
#else // _WIN32_WCE
		//TODO - figure out what to do here instead...
		::CloseHandle(hFile);

		return FALSE;
#endif // _WIN32_WCE
	}

	static DWORD CALLBACK StreamReadCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
	{
		_ASSERTE(dwCookie != 0);
		_ASSERTE(pcb != NULL);

		return !::ReadFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, NULL);
	}

	static DWORD CALLBACK StreamWriteCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
	{
		_ASSERTE(dwCookie != 0);
		_ASSERTE(pcb != NULL);

		return !::WriteFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, NULL);
	}

	LRESULT OnEnUpdateEditOutput(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(!IsWindow())
			return 0;
		int nTextLength = CRichEditCtrl::GetTextLengthEx();
		_Module.Enable(ID_EDIT_SELECT_ALL, (nTextLength == 0) ? false : true);
		return 0;
	}

	LRESULT OnBnClickedChcListen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnOutput(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
//		bHandled = FALSE;
		LPCTSTR lpszOutput = (LPCTSTR)wParam;
		if(lParam)
		{//Set the bold font
			CHARFORMAT2 cf;
			::ZeroMemory(&cf, sizeof(cf));
			cf.dwMask = CFM_BOLD;
			switch(lParam)
			{
			case bold_set://1
				cf.dwEffects = CFE_BOLD;
				break;
			case bold_reset://2
				cf.dwEffects = 0;
				break;
			case bold_no_change://0
			default: ATLASSERT(FALSE);
			}
			ATLVERIFY(CRichEditCtrl::SetSelectionCharFormat(cf));
		}
		CRichEditCtrl::AppendText(lpszOutput);
		return TRUE;
	}

	LRESULT OnCbnEditchangeCmbUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		CRichEditCtrl::SetReadOnly();

		//Recent document list
		int FILE_MENU_POSITION = 0;
		int RECENT_MENU_POSITION = 8;
#ifndef _WIN32_WCE
		WTL::CMenu menu(_Module.GetMenu());
		ATLASSERT(menu.IsMenu());
		WTL::CMenuHandle menuFile = menu.GetSubMenu(FILE_MENU_POSITION);
		ATLASSERT(menuFile.IsMenu());
#ifdef _DEBUG
		// absolute position, can change if menu changes
		CComBSTR bstrMenuString;
		menuFile.GetMenuString(RECENT_MENU_POSITION, bstrMenuString.m_str, MF_BYPOSITION);
		CString strMenuString;
		strMenuString.LoadString(IDS_RECENT_FILES);//_T("Recent &Files")
		ATLASSERT(strMenuString == bstrMenuString);
#endif //_DEBUG
		WTL::CMenuHandle menuMru = menuFile.GetSubMenu(RECENT_MENU_POSITION);
		ATLASSERT(menuMru.IsMenu());
		m_mru.SetMenuHandle(menuMru.m_hMenu);
		m_mru.ReadFromRegistry(CRegKeyPortScanner().GetKeyName());

		menu.Detach();
		menuFile.Detach();
		menuMru.Detach();
#endif // _WIN32_WCE

		return 0;
	}

	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(DoFileSaveAs())
		{
			_Module.UpdateTitle();
#ifndef _WIN32_WCE
			m_mru.AddToList(GetFilePath());
			m_mru.WriteToRegistry(CRegKeyPortScanner().GetKeyName());
#endif // _WIN32_WCE
		}
		return 0;
	}

#ifndef _WIN32_WCE
	LRESULT OnFileRecent(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// check if we have to save the current one
		if(!QueryClose())
		{
			if(!DoFileSaveAs())
				return 0;
		}

		// get file name from the MRU list
		TCHAR szFile[MAX_PATH];
		if(m_mru.GetFromList(wID, szFile, MAX_PATH))
		{
			// find file name without the path
			LPTSTR lpstrFileName = szFile;
			for(int i = lstrlen(szFile) - 1; i >= 0; i--)
			{
				if(szFile[i] == '\\')
				{
					lpstrFileName = &szFile[i + 1];
					break;
				}
			}
			// open file
			if(DoFileOpen(szFile, lpstrFileName))
				m_mru.MoveToTop(wID);
			else
				m_mru.RemoveFromList(wID);
			m_mru.WriteToRegistry(CRegKeyPortScanner().GetKeyName());
		}
		else
		{
			::MessageBeep(MB_ICONERROR);
		}

		return 0;
	}
#endif // _WIN32_WCE

	BOOL DoFileOpen(LPCTSTR lpstrFileName, LPCTSTR lpstrFileTitle)
	{
		BOOL bRet = LoadFile(lpstrFileName);
		if(bRet)
		{
			Init(lpstrFileName, lpstrFileTitle);
			_Module.UpdateTitle();
		}
		else
		{
			CString strMessage;
			strMessage.Format(IDS_LOAD_FILE_FAILED, lpstrFileName, ::GetLastErrorString());//Load "%s" file failed! %s
			::AfxMessageBox(strMessage, MB_ICONSTOP);
		}

		return bRet;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
//		bHandled = FALSE;

		DoFileNew();
/*
	//	ATLVERIFY(CRichEditCtrl::SetWindowText(NULL));//_T(""));
		CRichEditCtrl::SetSel(0, -1);
		CRichEditCtrl::Clear();
*/
		return 0;
	}

	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(m_strFilePath.IsEmpty())
		{//Save as
			if(!DoFileSaveAs())
				return 0;
			_Module.UpdateTitle();
		}
		else
		{
			if(!SaveFile(m_strFilePath))
				return 0;
			SetModify(FALSE);
		}
#ifndef _WIN32_WCE
		m_mru.AddToList(m_strFilePath);
		m_mru.WriteToRegistry(CRegKeyPortScanner().GetKeyName());
#endif // _WIN32_WCE
		return 0;
	}

	LRESULT OnListen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditSelectall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
