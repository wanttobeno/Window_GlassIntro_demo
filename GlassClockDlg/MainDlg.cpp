// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "aboutdlg.h"
#include "MainDlg.h"
#pragma comment(lib,"dwmapi")


/////////////////////////////////////////////////////////////////////////////
// Construction

CMainDlg::CMainDlg() : m_pLogoImage(NULL)
{
	SetThemeClassList ( L"globals" );
}


/////////////////////////////////////////////////////////////////////////////
// Message/command handlers

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnInitDialog ( HWND hwndFocus, LPARAM lParam )
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();

	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	// Add glass to the top of the window.
	if ( IsCompositionEnabled() )
	{
		MARGINS mar = {0};

		mar.cyTopHeight = m_cyGlassHeight;
		DwmExtendFrameIntoClientArea ( *this, &mar );
	}

	// Determine what font to use for the text.
	LOGFONT lf = {0};

	OpenThemeData();

	if ( !IsThemeNull() )
		GetThemeSysFont ( TMT_MSGBOXFONT, &lf );
	else
	{
		NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };

		SystemParametersInfo ( SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, false );
		lf = ncm.lfMessageFont;
	}

	lf.lfHeight *= 3;
	m_font.CreateFontIndirect ( &lf );

	// Set up the owner-draw static control
	m_wndTimeLabel.Attach ( GetDlgItem(IDC_CLOCK) );
	m_wndTimeLabel.ModifyStyle ( SS_TYPEMASK, SS_OWNERDRAW );

	// Other initialization
	EnableThemeDialogTexture ( ETDT_ENABLE );
	m_bLogoOn = true;
	m_bCompositionOn = IsCompositionEnabled();

	// Load the logo image.
	TCHAR szLogoPath[MAX_PATH];

	GetModuleFileName ( GetModuleHandle(NULL), szLogoPath, _countof(szLogoPath) );
	PathRemoveFileSpec ( szLogoPath );
	PathAppend ( szLogoPath, _T("mylogo.png") );
	m_pLogoImage = Image::FromFile ( CT2CW(szLogoPath) );

	// Start a 1-second timer so we update the clock every second.
	SetTimer ( 1, 1000 );

	return TRUE;
}

void CMainDlg::OnPaint ( HDC hdc )
{
	CPaintDC dc(*this);
	CRect rcGlassArea;

	if ( IsCompositionEnabled() )
	{
		GetClientRect ( rcGlassArea );
		rcGlassArea.bottom = m_cyGlassHeight;
		dc.FillSolidRect ( rcGlassArea, RGB(0,0,0) );
	}

	// Draw the logo
	if ( m_bLogoOn && NULL != m_pLogoImage )
	{
		Graphics gr(dc);

		gr.DrawImage ( m_pLogoImage, 0, 0 );
	}
}

void CMainDlg::OnTimer ( UINT uID, TIMERPROC pProc )
{
	if ( 1 != uID )
	{
		SetMsgHandled(false);
		return;
	}

	// Get the current time.
	TCHAR szTime[64];

	GetTimeFormat ( LOCALE_USER_DEFAULT, 0, NULL, NULL, szTime, _countof(szTime) );
	m_wndTimeLabel.SetWindowText ( szTime );
}

void CMainDlg::OnDrawItem ( UINT uID, LPDRAWITEMSTRUCT lpdis )
{
	CDCHandle dc = lpdis->hDC;
	CRect rcCtrl = lpdis->rcItem;
	CString sText;
	const UINT uFormat = DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX;

	if ( IDC_CLOCK != uID || ODT_STATIC != lpdis->CtlType )
	{
		SetMsgHandled(false);
		return;
	}

	m_wndTimeLabel.GetWindowText ( sText );

	if ( IsCompositionEnabled() )
	{
		// Set up a memory DC where we'll draw the text.
		CDC dcMem;

		dcMem.CreateCompatibleDC ( dc );
		dcMem.SaveDC();

		// Create a 32-bit bmp for use in offscreen drawing when glass is on
		BITMAPINFO dib = {0};
		CBitmap bmp;

		dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		dib.bmiHeader.biWidth = rcCtrl.Width();
		dib.bmiHeader.biHeight = -rcCtrl.Height();  // negative because DrawThemeTextEx() uses a top-down DIB
		dib.bmiHeader.biPlanes = 1;
		dib.bmiHeader.biBitCount = 32;
		dib.bmiHeader.biCompression = BI_RGB;

		bmp.CreateDIBSection ( dc, &dib, DIB_RGB_COLORS, NULL, NULL, 0 );

		// Set up the DC state.
		DTTOPTS dto = { sizeof(DTTOPTS) };

		dto.dwFlags = DTT_COMPOSITED|DTT_GLOWSIZE;
		dto.iGlowSize = 10;

		dcMem.SelectBitmap ( bmp );
		dcMem.SelectFont ( m_font );

		DrawThemeTextEx ( m_hTheme, dcMem, 0, 0, CT2CW(sText), -1, uFormat, rcCtrl, &dto );

		dc.BitBlt ( rcCtrl.left, rcCtrl.top, rcCtrl.Width(), rcCtrl.Height(), dcMem, 0, 0, SRCCOPY );

		dcMem.RestoreDC(-1);
	}
	else
	{
		dc.SaveDC();

		dc.FillSolidRect ( rcCtrl, GetSysColor(COLOR_3DFACE) );

		dc.SetBkMode ( TRANSPARENT );
		dc.SetTextColor ( GetSysColor(COLOR_WINDOWTEXT) );
		dc.SelectFont ( m_font );

		dc.DrawText ( CT2CW(sText), -1, rcCtrl, uFormat );

		dc.RestoreDC(-1);
	}
}

LRESULT CMainDlg::OnCompositionChanged ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// If composition is now on, tell DWM what part of our window should be glass.
	if ( IsCompositionEnabled() )
	{
		MARGINS mar = {0};

		mar.cyTopHeight = m_cyGlassHeight;
		DwmExtendFrameIntoClientArea ( *this, &mar );
	}

	return 0;
}

void CMainDlg::OnCancel ( UINT uCode, int nID, HWND hwndCtrl )
{
	CloseDialog(nID);
}

void CMainDlg::OnToggleLogo ( UINT uCode, int nID, HWND hwndCtrl )
{
	m_bLogoOn = !m_bLogoOn;
	RedrawWindow();
}

void CMainDlg::OnToggleComposition ( UINT uCode, int nID, HWND hwndCtrl )
{
	m_bCompositionOn = !m_bCompositionOn;
	DwmEnableComposition ( m_bCompositionOn );
}

void CMainDlg::OnAppAbout ( UINT uCode, int nID, HWND hwndCtrl )
{
	CAboutDlg dlg;

	dlg.DoModal(*this);
}


/////////////////////////////////////////////////////////////////////////////
// Other methods

void CMainDlg::CloseDialog ( int nVal )
{
	delete m_pLogoImage;
	DestroyWindow();
	PostQuitMessage(nVal);
}

// Wrapper for DwmIsCompositionEnabled() that's simpler to call.
bool CMainDlg::IsCompositionEnabled() const
{
	HRESULT hr;
	BOOL bEnabled;

	hr = DwmIsCompositionEnabled ( &bEnabled );
	return SUCCEEDED(hr) && bEnabled;
}
