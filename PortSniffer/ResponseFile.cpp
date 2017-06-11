//ResponseFile.cpp, implementation of the CResponseFile class.

#include "StdAfx.h"
#include "ResponseFile.h"
#include "FileManagement.h"//for MyCreateDirectory
#include "PortSnifferView.h"

//////////////////////////////////////////////////////////////////////
// Enable debug memory manager
#ifdef _DEBUG
#ifdef THIS_FILE
#undef THIS_FILE
#endif //THIS_FILE
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

CResponseFile::CResponseFile(void)
{
}

CResponseFile::~CResponseFile(void)
{
}

void CResponseFile::Close()
{
	if(!CMyFile::IsOpen())
		return;
//ATLTRACE(_T("\nFile close. Thread = %ld\n"), ::GetCurrentThreadId());
	CFile::Close();
}

void CResponseFile::Open(LPCTSTR lpszFileName, bool boWriteUnicodeBOM)
{
	if(CMyFile::IsOpen())
		return;

	CString strResponseFileName = lpszFileName;
	ATLASSERT(!strResponseFileName.IsEmpty());
	::MyCreateDirectory(strResponseFileName);
//DWORD dwThreadID = ::GetCurrentThreadId();
//ATLTRACE(_T("\nFile open \"%s\". Thread = %ld\n"), lpszFileName, dwThreadID);
	CMyFile::Open(strResponseFileName, CFile::modeCreate | CFile::typeBinary | CFile::modeWrite);// | CFile::shareCompat);
	if(boWriteUnicodeBOM)
		CMyFile::WriteUnicodeBOM();

	_Module.EnterCSOutput();

	//Bold font
	CString strReceive = _T("\nSave to ") + strResponseFileName + _T("\n");
	_Module.Output((LPTSTR)(LPCTSTR)strReceive, CPortSnifferView::bold_set);

	//no bold font
	strReceive.Empty();
	_Module.Output((LPTSTR)(LPCTSTR)strReceive, CPortSnifferView::bold_reset);

	_Module.LeaveCSOutput();
}

