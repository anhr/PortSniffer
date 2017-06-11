// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#define _CSTRING_NS	ATL
#include <atlstr.h>//for ATL::CString

#include <atlbase.h>
#include <atlapp.h>

#define _WTL_NO_CSTRING 
#include <atlmisc.h>//for WTL::CRect

#include "MyModule.h"

//extern CAppModule _Module;
extern CMyModule _Module;

#include <atlwin.h>

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include "Exception.h"

//detecting of the memory leaks 
#include "MyDumpMemoryLeaks.h"
//#define NO_USE_FILES

extern void log( LPCTSTR  lpszFormatString, ... );
extern const int g_nPort;

//#define CStringA CString
#define MY_DEFINES "..\..\..\PortSniffer\PortSniffer\PortSniffer\Defines.h"
#define RESOURCE "..\..\..\PortSniffer\PortSniffer\PortSniffer\resource.h"
#define USE_THREAD_CS
#include "EmailMe.h"

