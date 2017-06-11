//SocketAccept.cpp, implementation of the CAboutDlg class.

#include "StdAfx.h"
#include ".\SocketAccept.h"
#include "util_RF.h"
#include "Regexp.h"
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

//CMyCriticalSection CSocketAccept::m_csOutput;

CSocketAccept::CSocketAccept(CSocketAccept& sConnect, CArrayListen<CSocketAccept>* parrayAccepts)
	: CSocketTelnet(sConnect, false//false - do not assert (debugging breakpoint) during CException 
			, 1024 * 1024
		)
	, m_nConnectionID(parrayAccepts->GetCount())
	, m_parrayAccepts(parrayAccepts)
	, m_pSocketClient(new CSocketClient(this))
	, m_boReplaceHostHeader(false)
{
	CRegKeyPortScanner regKeyPortScanner;
	m_boReplaceHostHeader = regKeyPortScanner.IsReplaceHostHeader();
	Start();
}

CSocketAccept::~CSocketAccept(void)
{
//ATLTRACE("~CSocketAccept(). enter this = 0x%x\n", this);
	if(m_pSocketClient)
	{
		if(!m_pSocketClient->IsThreadCreated())//IsOpen())
		{
			if(!m_pSocketClient->IsDeleteThis())
			{
				delete m_pSocketClient;
				m_pSocketClient = NULL;
			}
		}
		else
			m_pSocketClient->SetSocketAccept();
	}
	if(!m_parrayAccepts)
		return;

	ATLASSERT(m_parrayAccepts);
	m_parrayAccepts->Remove(this);

	CString strStatus;
	strStatus.Format(IDS_SESSION_OPEN, m_parrayAccepts->GetCount());//%ld session(s) open
	_Module.Status(strStatus, 0);
//ATLTRACE("~CSocketAccept(). exit = 0x%x\n", this);
}

void CSocketAccept::Proc()
{
	_Module.EnterCSOutput();

	CString strStatus;
	strStatus.Format(IDS_ASSEPTED, m_nConnectionID);//"Session %ld starts."
	_Module.Status(strStatus);

	CString strOutput;

	//Bold font
	strOutput = "\r\n" + strStatus;
	CSocketTelnet::TraceSend(strOutput);
	_Module.Output((LPTSTR)(LPCTSTR)strOutput, CPortSnifferView::bold_set);

	//no bold font
	strOutput = _T("");
	CSocketTelnet::TraceSend(strOutput);
	_Module.Output((LPTSTR)(LPCTSTR)strOutput, CPortSnifferView::bold_reset);

	_Module.LeaveCSOutput();

	try
	{
		ATLASSERT(m_pSocketClient);
		if(!m_pSocketClient->Start())
		{//Stop listening
			_Module.Listen(false);
			CSocketBlocking::ThreadClear();
		}
		else
		{
			CSocketTelnet::Proc();
		}
	}
	catch(CException* e)
	{
		CString strMessage = e->GetErrorMessage();
		::log(strMessage);
		delete e;
		Cleanup();
	}

	_Module.EnterCSOutput();

	size_t sizeSessionsCount = m_parrayAccepts->GetCount();
	strStatus.Format(IDS_SESSION_STOPPED, m_nConnectionID);//"Session %ld stopped.");
	_Module.Status(strStatus);

	//Bold font
	strOutput = "\r\n" + strStatus;
	CSocketTelnet::TraceSend(strOutput);
	_Module.Output((LPTSTR)(LPCTSTR)strOutput, CPortSnifferView::bold_set);

	//no bold font
	strOutput = _T("");
	CSocketTelnet::TraceSend(strOutput);
	_Module.Output((LPTSTR)(LPCTSTR)strOutput, CPortSnifferView::bold_reset);

	_Module.LeaveCSOutput();
}

CString CSocketAccept::GetConnectionID()
{
	CString strConnectionID;
	strConnectionID.Format(_T("%d"), m_nConnectionID);
	return strConnectionID;
}

void CSocketAccept::OnReceive(int nErrorCode) 
{
	_Module.EnterCSOutput();
	OnReceiveCS(nErrorCode);
	_Module.LeaveCSOutput();
}

void CSocketAccept::OnReceiveCS(int nErrorCode) 
{
	try
	{
		if(m_pSocketClient)
			m_pSocketClient->CloseResponseFileName();

//		if(m_boReplaceHostHeader || m_regKeyPortScanner.IsSaveResponse())
		if(IsReceiveLine())
		{
			CSocketTelnet::OnReceive(nErrorCode);
			return;
		}
		CSocketBlocking::ReceiveErrorCode(nErrorCode);

		Receive();
	}
	catch(CFileException* e)
	{
		CString strError;
		e->GetErrorMessage(strError.GetBuffer(255), 255);
		strError.ReleaseBuffer();
		delete e;
		throw new CException(strError);
	}
}

CString CSocketAccept::Receive()
{
	int nRetCode = CSocketTelnet::ReceiveBuf();
	char* buf = CSocketTelnet::GetBufReceive();
//	if(!m_boReplaceHostHeader && m_pSocketClient)
	if(!IsReceiveLine() && m_pSocketClient)
		m_pSocketClient->Send(buf, nRetCode);

	//Bold font
	CString strReceive = "\r\n" + GetConnectionID() + "---<---";
	CSocketTelnet::TraceSend(strReceive);
	_Module.Output((LPTSTR)(LPCTSTR)strReceive, CPortSnifferView::bold_set);

	//no bold font
//	strReceive = (m_boReplaceHostHeader ? _T("") : CString(buf));
	strReceive = (IsReceiveLine() ? _T("") : CString(buf));
	CSocketTelnet::TraceSend(strReceive);
	_Module.Output((LPTSTR)(LPCTSTR)strReceive, CPortSnifferView::bold_reset);

	return buf;
}

bool CSocketAccept::ReceiveLine(LPCTSTR lpszResponse)
{
//ATLTRACE(_T("CSocketAccept::ReceiveLine(") + CString(lpszResponse) + _T(")\n"));
	bool boSaveResponse = m_regKeyPortScanner.IsSaveResponse();

	CStringA strResponse = lpszResponse;

	if(m_boReplaceHostHeader)
	{
		LPCSTR lpszHostHeader = "Host: ";
		if(strResponse.Find(lpszHostHeader) == 0)//Host: localhost
		{
			CString strHostAddress;
			_Module.GetHost(&strHostAddress);
			strResponse = lpszHostHeader + strHostAddress + CString(_T("\r\n"));
		}
		else if(strResponse.Find("Referer: ") == 0)//"Referer: http://localhost/Fax/Partnership.srf\r\n"
		{
			Regexp Re(_T("^(.*)(localhost)(.*)"), TRUE);
			ATLASSERT(Re.CompiledOK());
			USES_CONVERSION;
			if(Re.Match(A2T(strResponse)) && (Re.SubStrings() == 3))
			{
	#ifdef DEBUG
				CString str1 = Re[1];
				CString str2 = Re[2];
				CString str3 = Re[3];
	#endif //DEBUG
				CString strHostAddress;
				_Module.GetHost(&strHostAddress);
				strResponse = Re[1] + strHostAddress + Re[3];
			}
		}
	}
	if(boSaveResponse && (strResponse.Find("GET ") == 0))//GET /MyIsapi/PortSniffer.srf HTTP/1.1
	{
		Regexp Re(_T("^GET (.*) HTTP/.*"), TRUE);
		ATLASSERT(Re.CompiledOK());
		USES_CONVERSION;
		if(Re.Match(A2T(strResponse)) && (Re.SubStrings() == 1))
		{
			CString strFileName = Re[1];
//			if((strFileName != _T("/")) && !strFileName.IsEmpty())
			if(!strFileName.IsEmpty())
			{
				ATLASSERT(m_pSocketClient);

//				if(strFileName == _T("/"))
				if(strFileName[strFileName.GetLength() - 1] == _T('/'))
					strFileName = strFileName + _T("DefaultPage.txt");
				if(strFileName[0] != _T('/'))
					strFileName = _T("/") + strFileName;
				CString strFolder = m_regKeyPortScanner.GetApplacationDataFolder();// + _T("Output\\")
				if(!strFolder.IsEmpty() && (strFolder[strFolder.GetLength() - 1] != '\\'))
					strFolder += '\\';
				strFileName = strFolder + m_pSocketClient->GetHostAddress() + strFileName;
				strFileName.Replace(_T("/"), _T("\\"));
	//			::MyCreateDirectory(strFileName);

				if(m_pSocketClient)
					m_pSocketClient->SetResponseFileName(strFileName);
	/*
				if(m_file.IsOpen())
					m_file.Close();
				m_file.Open(strFileName, CFile::modeCreate | CFile::typeBinary);
	*/
			}
		}
	}
	if(m_pSocketClient)
		m_pSocketClient->Send(strResponse, strResponse.GetLength());

	CSocketTelnet::TraceSendA(strResponse);
	_Module.OutputA((LPSTR)(LPCSTR)strResponse);

	return false;
}

int CSocketAccept::Send(const char *lpszString, const int nSize)
{
	return CSocketTelnet::Write(lpszString, nSize);//::strlen(lpszString));
}

void CSocketAccept::OnClose(int nErrorCode)
{
	ATLTRACE(GetConnectionID() + " CSocketAccept::OnClose(...)\n");
	if(m_pSocketClient)
		m_pSocketClient->Stop(1000);

	CSocketTelnet::OnClose(nErrorCode);
}

int CSocketAccept::LastError(LPCTSTR lpszMessage, bool boException)
{
	int nLastError = CSocketBlocking::GetLastError(lpszMessage, false);
	return nLastError;
}

