//DlgPortScanner.CPP, implementation of the CDlgPortScanner class.

#include "StdAfx.h"
#include "DlgPortScanner.h"
#include "SocketBlocking\SocketListening.h"
#include "Defines.h"


//////////////////////////////////////////////////////////////////////
// Enable debug memory manager
#ifdef _DEBUG
#ifdef THIS_FILE
#undef THIS_FILE
#endif //THIS_FILE
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

int//width of the CDlgPortScanner control 
	CDlgPortScanner::Create(HWND hWndParent, LPTBBUTTON pTBButtons, int* pnIndex
#ifdef _DEBUG
		, const int nButtonCount
#endif //_DEBUG
	)
{
//	CString string;

	(*pnIndex)++;
	ATLASSERT(nButtonCount > *pnIndex);
	pTBButtons[*pnIndex].iString = -1;
	pTBButtons[*pnIndex].fsState = 0;//TBSTATE_ENABLED;
	pTBButtons[*pnIndex].fsStyle = TBSTYLE_BUTTON;//TBSTYLE_SEP;
	pTBButtons[*pnIndex].dwData = 0;
	pTBButtons[*pnIndex].iBitmap = -1;
	pTBButtons[*pnIndex].idCommand = WMU_DLG_PORT_SCANNER;
	CDialogImpl<CDlgPortScanner>::Create(hWndParent);
	CRect rectRec;
	ATLVERIFY(CDialogImpl<CDlgPortScanner>::GetWindowRect(&rectRec));
	return rectRec.Width();
}

LRESULT CDlgPortScanner::OnEnChangeEditListenPort(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Enable();

	return 0;
}

LRESULT CDlgPortScanner::OnCbnEditchangeCmbUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Enable();

	return 0;
}

void CDlgPortScanner::SetPort()
{
/*
	DWORD dwPort = m_comboBoxURL.GetItemData(m_comboBoxURL.GetCurSel());
	ATLASSERT(dwPort);
	ATLVERIFY(CDialogImpl<CDlgPortScanner>::SetDlgItemInt(IDC_EDIT_LISTEN_PORT, dwPort, FALSE));
*/
	CItemData* pItemData = (CItemData*)m_comboBoxURL.GetItemData(m_comboBoxURL.GetCurSel());
	ATLASSERT(pItemData);

	DWORD dwPortLocal = pItemData->GetPortLocal();
	ATLASSERT(dwPortLocal);
	ATLVERIFY(CDialogImpl<CDlgPortScanner>::SetDlgItemInt(IDC_EDIT_LISTEN_PORT, dwPortLocal, FALSE));

	DWORD dwPortRemote = pItemData->GetPortRemote();
	if(dwPortRemote == 0)
		dwPortRemote = dwPortLocal;
	ATLVERIFY(CDialogImpl<CDlgPortScanner>::SetDlgItemInt(IDC_EDIT_REMOTE_PORT, dwPortRemote, FALSE));
}

LRESULT CDlgPortScanner::OnCbnSelchangeCmbUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetPort();
	return 0;
}
