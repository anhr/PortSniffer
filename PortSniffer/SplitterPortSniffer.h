// SplitterPortSniffer.h : interface of the CSplitterPortSniffer class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <atlsplit.h>//for CHorSplitterWindow

#define SPLITTER_POS_DEFAULT 90 /*banner*/ + 18 /*border*/ * 2

template <bool t_bVertical = true>
class CMySplitterWindowT : public CSplitterWindowImpl<CMySplitterWindowT<t_bVertical>, t_bVertical>
{
public:
	CMySplitterWindowT()
		: m_nHeightMin(0)
	{
		CSplitterWindowImpl<CMySplitterWindowT<t_bVertical>, t_bVertical>::SetSplitterExtendedStyle(0);
	}
	DECLARE_WND_CLASS_EX(_T("WTL_SplitterWindow"), CS_DBLCLKS, COLOR_WINDOW)
	bool SetSplitterPos(int xyPos = -1, bool bUpdate = true)
	{
		if((xyPos != 0) && (xyPos < m_nHeightMin))
			xyPos = m_nHeightMin;
		return CSplitterWindowImpl<CMySplitterWindowT<t_bVertical>, t_bVertical>::SetSplitterPos(xyPos, bUpdate);
	//	return CHorSplitterWindow::SetSplitterPos(xyPos, bUpdate);
	}
	void SetSplitterPosDefault(int nHeightPaneBanner)
	{
		ATLASSERT(m_nHeightMin == 0);
		m_nHeightMin = SPLITTER_POS_DEFAULT + nHeightPaneBanner;
		SetSplitterPos(m_nHeightMin);
	}
private:
	int m_nHeightMin;
};
typedef CMySplitterWindowT<false>   CSplitterPortSniffer;
/*
class CSplitterPortSniffer
	: public CMySplitterWindowT<false>
//	: public CHorSplitterWindow
//	: public CSplitterWindowImpl<CSplitterPortSniffer<false>, false>
{
public:
	CSplitterPortSniffer(void);
	virtual ~CSplitterPortSniffer(void);
private:
	bool SetSplitterPos(int xyPos = -1, bool bUpdate = true);
};
*/