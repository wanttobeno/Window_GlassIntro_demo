// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMainDlg : public CDialogImpl<CMainDlg>, public CMessageFilter,
                 public CThemeImpl<CMainDlg>
{
public:
    enum { IDD = IDD_MAINDLG };
    // Construction
    CMainDlg();

    // Maps
    BEGIN_MSG_MAP(CMainDlg)
        CHAIN_MSG_MAP(CThemeImpl<CMainDlg>)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_DRAWITEM(OnDrawItem)
        MESSAGE_HANDLER_EX(WM_DWMCOMPOSITIONCHANGED, OnCompositionChanged)
        COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
        COMMAND_ID_HANDLER_EX(IDC_TOGGLE_LOGO, OnToggleLogo)
        COMMAND_ID_HANDLER_EX(IDC_TOGGLE_COMPOSITION, OnToggleComposition)
        COMMAND_ID_HANDLER_EX(ID_APP_ABOUT, OnAppAbout)
    END_MSG_MAP()

    // Message/command handlers
    BOOL PreTranslateMessage(MSG* pMsg);

    BOOL    OnInitDialog(HWND hwndFocus, LPARAM lParam);
    void    OnPaint(HDC hdc);
    void    OnTimer(UINT uID, TIMERPROC pProc);
    void    OnDrawItem(UINT uID, LPDRAWITEMSTRUCT lpdis);
    LRESULT OnCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void    OnCancel(UINT uCode, int nID, HWND hwndCtrl);
    void    OnToggleLogo(UINT uCode, int nID, HWND hwndCtrl);
    void    OnToggleComposition(UINT uCode, int nID, HWND hwndCtrl);
    void    OnAppAbout(UINT uCode, int nID, HWND hwndCtrl);

protected:
    CFont   m_font;
    CStatic m_wndTimeLabel;
    Image*  m_pLogoImage;

    bool m_bLogoOn, m_bCompositionOn;

    void CloseDialog(int nVal);
    bool IsCompositionEnabled() const;
    static const int m_cyGlassHeight = 150;
};
