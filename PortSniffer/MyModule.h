//MyModule.h

#pragma once

#include <atlstr.h>//for CString
#include "MyCriticalSection.h"

class CMyModule :
	public CAppModule
{
public:
	CMyModule(void);
	virtual ~CMyModule(void);
	void SetMainFrame(CWindow* pMainFrame)
	{ ATLASSERT(pMainFrame && pMainFrame->IsWindow()); m_pMainFrame = pMainFrame;}
	CWindow* GetMainFrame()
	{ return m_pMainFrame;}
	void CloseApp();
	void RefreshBanner();
	void ContextMenu(WPARAM wParam, LPARAM lParam);

	void Status(LPCTSTR lpszStatus, int nPart = 1);
	void Status(UINT nID);

	//не хочу чтобы одновременно несколько нитей писали в окно output
	void EnterCSOutput()
	{ m_csOutput.EnterCriticalSection(); }
	void LeaveCSOutput()
	{ m_csOutput.LeaveCriticalSection(); }
private:
	CMyCriticalSection m_csOutput;
public:

	//Output
	void OutputA(LPSTR lpszOutput);
	void OutputW(LPWSTR lpszOutput
			, UINT nBold = 0//CPortScannerView::Bold = bold_no_change
		);
#ifdef UNICODE
#define Output  OutputW
#else
#define Output  OutputA
#endif // !UNICODE
private:
	void OutputT(LPCTSTR lpszOutput
			, UINT nBold = 0//CPortScannerView::Bold = bold_no_change
		);
public:

	void GetHost(CString* pstrURL, UINT* pnPortLocal = NULL, UINT* pnPortRemote = NULL);
	void Listen(bool boQuitApp = true);
	CMenuHandle GetMenu() const;
	void UpdateTitle();

	//Enable of the menu item and tool bar buttom
	void Enable
		(
			UINT nID//menu item and tool bar buttom identified
			, bool boEnable
		);

	void HelpFinder(LPCTSTR lpszTopic);
private:
	CWindow* m_pMainFrame;
};
