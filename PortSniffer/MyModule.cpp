//MyModule.cpp

#include "StdAfx.h"
#include "MyModule.h"
#include "Defines.h"
#include "MyToolBarCtrl.h"
#include "MyStatusBarCtrl.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////
// Enable debug memory manager
#ifdef _DEBUG
#ifdef THIS_FILE
#undef THIS_FILE
#endif //THIS_FILE
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

CMyModule::CMyModule(void)
	: m_pMainFrame(NULL)
//	, m_boDestroy(false)
{
}

CMyModule::~CMyModule(void)
{
}

void CMyModule::Status(LPCTSTR lpszStatus, int nPart)
{
	ATLASSERT(m_pMainFrame && m_pMainFrame->IsWindow());
	CMyStatusBarCtrl::Status(m_pMainFrame, lpszStatus, nPart);
}

void CMyModule::Status(UINT nID)
{
	CString strStatus;
	strStatus.LoadString(nID);
	Status(strStatus);
}

void CMyModule::OutputA(LPSTR lpszOutput)
{
	USES_CONVERSION;
	OutputT(A2T(lpszOutput));
}

void CMyModule::OutputW(LPWSTR lpszOutput, UINT nBold)
{
	USES_CONVERSION;
	OutputT(W2T(lpszOutput), nBold);
}

void CMyModule::OutputT(LPCTSTR lpszOutput, UINT nBold)
{
	ATLASSERT(m_pMainFrame && m_pMainFrame->IsWindow());
	ATLVERIFY(m_pMainFrame->SendMessage(WMU_OUTPUT, (WPARAM)lpszOutput, nBold));
}

void CMyModule::HelpFinder(LPCTSTR lpszTopic)
{
	ATLASSERT(m_pMainFrame && m_pMainFrame->IsWindow());
	ATLVERIFY(m_pMainFrame->SendMessage(WMU_HELP_FINDER, (WPARAM)lpszTopic, 0));
}

void CMyModule::GetHost(CString* pstrURL, UINT* pnPortLocal, UINT* pnPortRemote)
{
	ATLASSERT(m_pMainFrame && m_pMainFrame->IsWindow());
//*pstrURL = _T("bonalink.com");
//*pnPort = 80;
	ATLVERIFY(m_pMainFrame->SendMessage(WMU_URL, (WPARAM)pstrURL, 0));
	if(pnPortLocal)
		ATLVERIFY(m_pMainFrame->SendMessage(WMU_PORT_LOCAL, (WPARAM)pnPortLocal, 0));
	if(pnPortRemote)
		ATLVERIFY(m_pMainFrame->SendMessage(WMU_PORT_REMOTE, (WPARAM)pnPortRemote, 0));
}

void CMyModule::Listen(bool boQuitApp)
{
	ATLASSERT(m_pMainFrame && m_pMainFrame->IsWindow());
//	ATLVERIFY(m_pMainFrame->PostMessage(WMU_LISTEN, 0//(WPARAM)boListen
//		, 0));
//	ATLVERIFY(m_pMainFrame->SendMessage(WM_COMMAND, ID_LISTEN, 0));
	//Еслия буду использовать SendMessage то не получится закрыть сокет в случае неудачного соединения с сервером
	//Произойдет исключение в m_thread.WaitThreadIsCreated(1000); в функции CSocketBlocking::Stop()
	//потомучто получается я буду ждать закрытия нити, в которой нахожусь
	ATLVERIFY(m_pMainFrame->PostMessage(WM_COMMAND, ID_LISTEN, boQuitApp ? 0 : 1));
}

void CMyModule::Enable(UINT nID, bool boEnable)
{
	ATLASSERT(m_pMainFrame && m_pMainFrame->IsWindow());
	CMyToolBarCtrl::Enable(m_pMainFrame, nID, boEnable);
}

CMenuHandle CMyModule::GetMenu() const
{
	ATLASSERT(m_pMainFrame && m_pMainFrame->IsWindow());
//	return m_pMainFrame->GetMenu();
	HMENU hMenu;
	ATLVERIFY(m_pMainFrame->SendMessage(WMU_MENU, (WPARAM)&hMenu, 0));
	return hMenu;
}

void CMyModule::UpdateTitle()
{
	ATLASSERT(m_pMainFrame && m_pMainFrame->IsWindow());
	ATLVERIFY(m_pMainFrame->SendMessage(WMU_UPDATE_TITLE, (WPARAM)0, 0));
}

void CMyModule::CloseApp()
{
	ATLASSERT(m_pMainFrame && m_pMainFrame->IsWindow());
	m_pMainFrame->PostMessage(WM_CLOSE);
}

void CMyModule::RefreshBanner()
{
	ATLASSERT(m_pMainFrame && m_pMainFrame->IsWindow());
	ATLVERIFY(m_pMainFrame->PostMessage(WMU_REFRESH_BANNER, (WPARAM)0, 0));
}

void CMyModule::ContextMenu(WPARAM wParam, LPARAM lParam)
{
	ATLASSERT(m_pMainFrame && m_pMainFrame->IsWindow());
	ATLVERIFY(m_pMainFrame->SendMessage(WMU_CONTEXT_MENU, wParam, lParam));
}
		
