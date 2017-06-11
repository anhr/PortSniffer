// PortSniffer.cpp : main source file for PortSniffer.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlsplit.h>

#include "resource.h"

#include "PortSnifferView.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include <strsafe.h>//for StringCchPrintf
#include "util_RF.h"

//////////////////////////////////////////////////////////////////////
// Enable debug memory manager
#ifdef _DEBUG
#ifdef THIS_FILE
#undef THIS_FILE
#endif //THIS_FILE
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

//CAppModule _Module;
CMyModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	CMyToolBarCtrl::RegisterWindowMessages();

	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	HMODULE hInstRich = ::LoadLibrary(CRichEditCtrl::GetLibraryName());
	ATLASSERT(hInstRich != NULL);

	CRegKeyPortScanner regKeyPortScanner;
	regKeyPortScanner.Init();

	int nRet = Run(lpstrCmdLine, nCmdShow);

	::FreeLibrary(hInstRich);

	_Module.Term();
	::CoUninitialize();

	MyDumpMemoryLeaks();

	return nRet;
}

void log( LPCTSTR  lpszFormatString, ... )//const char* s)
{
	va_list     varg_ptr = NULL;
	SYSTEMTIME  SystemTime;
	HRESULT hr = S_OK;
	BOOL bRetVal = FALSE;
	// szOutputString is the output string
	TCHAR       szOutputString[1024] = {0};
	DWORD       cb;

	// Initialize the buffer
	ZeroMemory(szOutputString, sizeof(szOutputString));

	// Get the current time
	GetLocalTime(&SystemTime);
	hr = StringCchPrintf(szOutputString,1024,  _T("%02d.%02d.%04d %02d:%02d:%02d.%03d:\n"), 
									SystemTime.wMonth,
									SystemTime.wDay,
									SystemTime.wYear,
									SystemTime.wHour,
									SystemTime.wMinute,
									SystemTime.wSecond, 
									SystemTime.wMilliseconds);
	if(hr != S_OK)
	{
		log(_T("StringCchPrintf failed, hr = 0x%x for szOutputString"), hr );                
		goto Exit;
	}       
	cb = lstrlen(szOutputString);

	va_start(varg_ptr, lpszFormatString);
	hr = StringCchVPrintf(&szOutputString[cb],(sizeof(szOutputString)/sizeof(szOutputString[0])) - cb -1,
									lpszFormatString,
									varg_ptr); 
	if(hr != S_OK)
	{
		log(_T("StringCchVPrintf failed, hr = 0x%x for szOutputString"), hr );                
		goto Exit;
	}

	// Write the string to the debugger
	{
		CString strOutputString(szOutputString);
		strOutputString.Replace(_T("\r"), _T(""));
		strOutputString.Replace(_T("\n"), _T(""));
		strOutputString += _T("\r\n");
		printf("%s",strOutputString);
		OutputDebugString(strOutputString);
	}

	Exit:
	if(varg_ptr)
		va_end(varg_ptr);

}

extern const int g_nPort = 0;
