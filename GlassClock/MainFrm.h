// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CMessageFilter,
	public CThemeImpl<CMainFrame>
{
public:
	DECLARE_FRAME_WND_CLASS_EX(NULL, IDR_MAINFRAME, CS_HREDRAW|CS_VREDRAW, COLOR_WINDOW)

	// Construction
	CMainFrame();

	// Maps
	BEGIN_MSG_MAP(CMainFrame)
		CHAIN_MSG_MAP(CThemeImpl<CMainFrame>)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		MSG_WM_CLOSE(OnClose)
		MESSAGE_HANDLER_EX(WM_DWMCOMPOSITIONCHANGED, OnCompositionChanged)
		COMMAND_ID_HANDLER_EX(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER_EX(ID_APP_ABOUT, OnAppAbout)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	// Message/command handlers
	BOOL PreTranslateMessage(MSG* pMsg);

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	BOOL    OnEraseBkgnd(HDC hdc);
	void    OnTimer(UINT uID, TIMERPROC pProc);
	void    OnGetMinMaxInfo(MINMAXINFO* lpmmi);
	void    OnClose();
	LRESULT OnCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void    OnFileExit(UINT uCode, int nID, HWND hwndCtrl);
	void    OnAppAbout(UINT uCode, int nID, HWND hwndCtrl);

protected:
	CFont m_font;
	bool IsCompositionEnabled() const;
	static const int m_cyGlassHeight = 100;
};
