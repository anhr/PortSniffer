// ResponseFile.h : interface of the CResponseFile class

#pragma once
#include "mfc\myfile.h"

class CResponseFile :
	private CMyFile
{
public:
	CResponseFile(void);
	virtual ~CResponseFile(void);
	void Open(LPCTSTR lpszFileName, bool boWriteUnicodeBOM);
//	BOOL IsOpen()
//	{ return CMyFile::IsOpen(); }
	void Write(LPCTSTR lpszString)
	{
//ATLTRACE(_T("\nFile write \"%s\". Thread = %ld\n"), lpszString, ::GetCurrentThreadId());
		CMyFile::Write(lpszString);
	}
	void Close();
	void Write(const void* lpBuf, UINT nCount)
	{ CFile::Write(lpBuf, nCount);}
};
