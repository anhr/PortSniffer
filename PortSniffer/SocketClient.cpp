//SocketClient.cpp, implementation of the CAboutDlg class.

#include "StdAfx.h"
#include "SocketClient.h"
#include "resource.h"
#include "AfxMessageBox.h"
#include "RegKeyPortScanner.h"
#include "SocketAccept.h"
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

UINT CSocketClient::m_nClientsCount = 0;
bool CSocketClient::m_boQuitApp = false;

CSocketClient::CSocketClient(CSocketAccept* psocketAccept)
	: CSocketTelnet(TRUE//false - do not assert (debugging breakpoint) during CException 
				, 1024 * 1024
		)
	, m_psocketAccept(psocketAccept)
	, m_boDeleteThis(false)
	, m_contentType(content_type_unknown)//0
	, m_boSaveResponseToFile(false)
//	, m_boSaveResponseToFile2(false)
	, m_boFirstEmptyLine(true)
{
	m_nClientsCount++;
}

CSocketClient::~CSocketClient(void)
{
//	if(m_file.IsOpen())
	m_file.Close();

	ATLASSERT(!m_boDeleteThis);
	m_boDeleteThis = true;
//ATLTRACE("~CSocketClient(). enter this = 0x%x\n", this);
	if(m_psocketAccept)
		m_psocketAccept->SetSocketClient();
	
	ATLASSERT(m_nClientsCount != 0);
	m_nClientsCount--;
	if(m_nClientsCount == 0)
	{
		if(m_boQuitApp)
		{
			_Module.CloseApp();
		}
	}

	CString str;
	str.Format(_T("CSocketClient::~CSocketClient(void) this = 0x%x. Clients count = %ld"), this, m_nClientsCount);
	::log(str);

//ATLTRACE("~CSocketClient(). exit this = 0x%x\n", this);
}

bool CSocketClient::Start()
{
	UINT nPortLocal = 0;
	UINT nPortRemote = 0;
	_Module.GetHost(&m_strHostAddress, &nPortLocal, &nPortRemote);
	ATLASSERT(nPortRemote);

	LPCTSTR lpszProtocol;

	lpszProtocol = _T("http://");
	if(m_strHostAddress.Find(lpszProtocol) == 0)
		m_strHostAddress.Replace(lpszProtocol, _T(""));

	lpszProtocol = _T("ftp://");
	if(m_strHostAddress.Find(lpszProtocol) == 0)
		m_strHostAddress.Replace(lpszProtocol, _T(""));

	lpszProtocol = _T("https://");
	if(m_strHostAddress.Find(lpszProtocol) == 0)
		m_strHostAddress.Replace(lpszProtocol, _T(""));

	WSAInit();

	//Create destination socket
	CSocketTelnet::Create();
	CSockAddr saServer(INADDR_ANY, 0);
	CSocketBlocking::Bind(saServer);
	switch(CSocketBlocking::Connect(m_strHostAddress, nPortRemote, false))
	{
	default:
	case 10022:
		{
			//An invalid argument was supplied.
			CSocketBlocking::Cleanup();

			CString strMessage;
			strMessage.Format(IDS_HOST_FAILED, m_strHostAddress, nPortRemote);//Connect to %s host %d port failed!
			::AfxMessageBox(strMessage, MB_ICONSTOP, _Module.GetMainFrame()->m_hWnd);

			return false;
		}
	case 0: break;
	}
	CSocketBlocking::Start();

	CRegKeyPortScanner regKeyPortScanner;
	regKeyPortScanner.SetHost(m_strHostAddress, nPortLocal, nPortRemote);
	return true;
}

void CSocketClient::SetResponseFileName(LPCTSTR lpszResponseFileName)
{
//ATLTRACE(_T("\nCSocketClient::SetResponseFileName(%s)\n"), lpszResponseFileName);
	m_strResponseFileName = lpszResponseFileName;
//	m_boSaveResponseToFile2 = false;
	m_boFirstEmptyLine = true;
	m_contentType = content_type_unknown;//0
}

void CSocketClient::OnReceive(int nErrorCode) 
{
	_Module.EnterCSOutput();
	OnReceiveCS(nErrorCode);
	_Module.LeaveCSOutput();
}

void CSocketClient::OnReceiveCS(int nErrorCode) 
{
	try
	{
		if(IsSaveResponseToFile())
		{
			ReceiveErrorCode(nErrorCode);

			int nRetCode = CSocketTelnet::ReceiveBuf();
			char* buf = CSocketTelnet::GetBufReceive();
			char* pLineBegin = buf;
			for(char* pBufCur = buf; pBufCur < (buf + nRetCode); pBufCur++)
			{
				if((*pBufCur == '\r') || (*pBufCur == '\n') || (pBufCur >= (buf + nRetCode - 1)))
				{//end of line
					if((pBufCur < (buf + nRetCode - 1)) && ((*(pBufCur + 1) == '\r') || (*(pBufCur + 1) == '\n')))
						pBufCur++;
					pBufCur++;
					CString strResponse(pLineBegin, pBufCur - pLineBegin);
//OutputDebugString(strResponse);

					ReceiveLine(strResponse);
	//				if(!m_boSaveResponseToFile2 && m_boSaveResponseToFile)
					if(m_boSaveResponseToFile)
					{
	/*
						if(m_contentType != content_type_text)
							m_boSaveResponseToFile2 = true;
	*/
						if(m_boFirstEmptyLine)
						{
							m_file.Close();
						}
						else
						{
							m_file.Open(m_strResponseFileName, m_contentType == content_type_text);
							if(m_contentType == content_type_text)
								m_file.Write(strResponse);
							else
							{
								m_file.Write(pLineBegin, nRetCode - (pLineBegin - buf));
								break;
//								return;
							}
						}
						m_boFirstEmptyLine = false;
					}
					pLineBegin = pBufCur;
				}//end of line
//				pBufCur++;
			}//for(int i = 0; i < nRetCode; i++)
/*
			CString strReceive = CString(buf);

			CString strResponse1 = m_strResponseTail + strReceive;//Receive();
			strResponse1.Replace(_T("\r\n"), _T("\n"));
			strResponse1.Replace(_T("\r"), _T("\n"));
			CString strResponse;
			while(!strResponse1.IsEmpty())
			{
				int index = strResponse1.Find(_T("\n"));
				if(index != -1)
				{
					strResponse = strResponse1.Left(index + 0) + _T("\r\n");
					strResponse1 = strResponse1.Right(strResponse1.GetLength() - (index + 1));
					m_strResponseTail.Empty();
				}
				else
				{
					strResponse = strResponse1;
					if(strResponse.Find(_T("\r\n")) == -1)
					{
						m_strResponseTail = strResponse;
						break;
					}
					strResponse1.Empty();
				}
				ReceiveLine(strResponse);
//				if(!m_boSaveResponseToFile2 && m_boSaveResponseToFile)
				if(m_boSaveResponseToFile)
				{
/*
					if(m_contentType != content_type_text)
						m_boSaveResponseToFile2 = true;
*/
/*
					if(m_boFirstEmptyLine)
					{
						m_file.Close();
					}
					else
					{
						m_file.Open(m_strResponseFileName, m_contentType == content_type_text);
						if(m_contentType == content_type_text)
							m_file.Write(strResponse);
						else
m_file.Write(strResponse1);
					}
					m_boFirstEmptyLine = false;
				}
			}
			if(m_contentType == content_type_text)
			{
				CString str = strResponse;
				str.Replace(_T("\r"), _T(""));
				str.Replace(_T("\n"), _T(""));
				if(str.GetLength() == strResponse.GetLength())
					m_file.Write(strResponse);
			}
*/
			RelayToAccept(nRetCode, buf);
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

CString CSocketClient::RelayToAccept(int nRetCode, char* buf)
{
	if(m_psocketAccept)
		m_psocketAccept->Send(buf, nRetCode);

	//Bold font
	CString strReceive = "\r\n" + (m_psocketAccept ? m_psocketAccept->GetConnectionID() : "") + "--->---";
	CSocketTelnet::TraceSend(strReceive);
	_Module.Output((LPTSTR)(LPCTSTR)strReceive, CPortSnifferView::bold_set);

	//no bold font
	strReceive = CString(buf);
	CSocketTelnet::TraceSend(strReceive);
	_Module.Output((LPTSTR)(LPCTSTR)strReceive, CPortSnifferView::bold_reset);

	return strReceive;
}

/*
CString CSocketClient::Receive()
{
	int nRetCode = CSocketTelnet::ReceiveBuf();
	char* buf = CSocketTelnet::GetBufReceive();
	if(m_psocketAccept)
		m_psocketAccept->Send(buf, nRetCode);

	//Bold font
	CString strReceive = "\r\n" + (m_psocketAccept ? m_psocketAccept->GetConnectionID() : "") + "--->---";
	CSocketTelnet::TraceSend(strReceive);
	_Module.Output((LPTSTR)(LPCTSTR)strReceive, CPortSnifferView::bold_set);

	//no bold font
	strReceive = CString(buf);
	CSocketTelnet::TraceSend(strReceive);
	_Module.Output((LPTSTR)(LPCTSTR)strReceive, CPortSnifferView::bold_reset);

	return strReceive;
}
*/

bool CSocketClient::ReceiveLine(LPCTSTR lpszResponse)
{
//ATLTRACE(_T("CSocketClient::ReceiveLine(") + CString(lpszResponse) + _T(")\n"));

	CStringA strResponse = lpszResponse;

	strResponse.Replace("\r", "");
	strResponse.Replace("\n", "");
	if(strResponse.IsEmpty())
	{
//		ATLASSERT(!m_boSaveResponseToFile);
		m_boSaveResponseToFile = true;
//ATLTRACE(_T("m_boSaveResponseToFile = true;\n"));
	}
	else
	{
		strResponse.MakeLower();
		LPCSTR lpszContentTypeHeader = "content-type: ";
		if(strResponse.Find(lpszContentTypeHeader) == 0)
		{
			CString strlpszContentType = strResponse.Right(strResponse.GetLength() - ::strlen(lpszContentTypeHeader));
			if(strlpszContentType.Find(_T("text")) == 0)
				m_contentType = content_type_text;
		}
	}
	return false;
}

void CSocketClient::CloseResponseFileName()
{
	m_boSaveResponseToFile = false;
	m_strResponseFileName.Empty();

	//Здесь нельзя закрывать файл потому что в другой нити он может все еще использоваться.
	//Лучше его закрывать непосредственно перед открытием ноавого файла в другой нити в CSocketClient::OnReceive(int nErrorCode) 
	/*
	if(m_file.IsOpen())
	{
ATLTRACE(_T("\nFile close. Thread = %ld\n"), ::GetCurrentThreadId());
		m_file.Close();
	}
	*/
}

void CSocketClient::OnClose(int nErrorCode)
{
	ATLTRACE(//GetConnectionID() + 
		" CSocketClient::OnClose(...)\n");
	if(m_psocketAccept)
		m_psocketAccept->Stop(false);

	CSocketTelnet::OnClose(nErrorCode);

}

void CSocketClient::DeleteThis()
{
	if(!m_boDeleteThis)
		delete this;
}
