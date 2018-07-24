// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#pragma comment(lib,"dwmapi")


/////////////////////////////////////////////////////////////////////////////
// Construction

CMainFrame::CMainFrame()
{
	SetThemeClassList ( L"globals" );
}


/////////////////////////////////////////////////////////////////////////////
// Message/command handlers

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}

LRESULT CMainFrame::OnCreate ( LPCREATESTRUCT lpcs )
{
	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();

	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	// Add glass to the bottom of the frame.
	if ( IsCompositionEnabled() )
	{
		MARGINS mar = {0};

		mar.cyBottomHeight = m_cyGlassHeight;
		DwmExtendFrameIntoClientArea ( *this, &mar );
	}

	// Determine what font to use for the text.
	LOGFONT lf = {0};

	if ( !IsThemeNull() )
		GetThemeSysFont ( TMT_MSGBOXFONT, &lf );
	else
	{
		NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };

		SystemParametersInfo ( SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, false );
		lf = ncm.lfMessageFont;
	}

	m_font.CreateFontIndirect ( &lf );

	// Start a 1-second timer so we update the clock every second.
	SetTimer ( 1, 1000 );

	return 0;
}

BOOL CMainFrame::OnEraseBkgnd ( HDC hdc )
{
	CDCHandle dc = hdc;
	CRect rcClient, rcText;

	// Fill the whole client area (including the glass area) with black. This
	// makes the glass show through since all the alpha values will be 0.
	GetClientRect ( rcClient );
	dc.FillSolidRect ( rcClient, RGB(0,0,0) );

	rcText = rcClient;
	rcText.top = rcText.bottom - m_cyGlassHeight;

	// Get the current time.
	TCHAR szTime[64];

	GetTimeFormat ( LOCALE_USER_DEFAULT, 0, NULL, NULL, szTime, _countof(szTime) );

	// Draw the time in the glass area. The drawing code we use is different
	// based on whether composition is active or not.
	if ( IsCompositionEnabled() )
	{
		// Set up a memory DC and bitmap that we'll draw into
		CDC dcMem;
		CBitmap bmp;
		BITMAPINFO dib = {0};

		dcMem.CreateCompatibleDC ( dc );
		dcMem.SaveDC();

		dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		dib.bmiHeader.biWidth = rcText.Width();
		dib.bmiHeader.biHeight = -rcText.Height();  // negative because DrawThemeTextEx() needs a top-down bitmap
		dib.bmiHeader.biPlanes = 1;
		dib.bmiHeader.biBitCount = 32;
		dib.bmiHeader.biCompression = BI_RGB;

		bmp.CreateDIBSection ( dc, &dib, DIB_RGB_COLORS, NULL, NULL, 0 );

		dcMem.SelectBitmap ( bmp );
		dcMem.SelectFont ( m_font );

		// Draw the text!
		DTTOPTS dto = { sizeof(DTTOPTS) };
		const UINT uFormat = DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX;
		CRect rcText2 = rcText;         // same as rcText but with (0,0) as the top-left

		dto.dwFlags = DTT_COMPOSITED|DTT_GLOWSIZE;
		dto.iGlowSize = 10;
		rcText2 -= rcText2.TopLeft();

		DrawThemeTextEx ( m_hTheme, dcMem, 0, 0, CT2CW(szTime), -1, uFormat, rcText2, &dto );

		// Blit the text to the screen.
		BitBlt ( dc, rcText.left, rcText.top, rcText.Width(), rcText.Height(), dcMem, 0, 0, SRCCOPY );

		// Cleanup
		dcMem.RestoreDC(-1);
	}
	else
	{
		const UINT uFormat = DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX;

		// Set up the DC
		dc.SaveDC();
		dc.SetTextColor ( RGB(255,255,255) );
		dc.SelectFont ( m_font );
		dc.SetBkMode ( TRANSPARENT );

		// Draw the text!
		dc.DrawText ( szTime, -1, rcText, uFormat );

		// Cleanup
		dc.RestoreDC(-1);
	}

	return true;    // we drew the entire background
}

void CMainFrame::OnTimer ( UINT uID, TIMERPROC pProc )
{
	if ( 1 != uID )
	{
		SetMsgHandled(false);
		return;
	}

	RedrawWindow();
}

void CMainFrame::OnGetMinMaxInfo ( MINMAXINFO* lpmmi )
{
	lpmmi->ptMinTrackSize.x = 200;
	lpmmi->ptMinTrackSize.y = 200;
}

void CMainFrame::OnClose()
{
	KillTimer(1);
	SetMsgHandled(false);
}

LRESULT CMainFrame::OnCompositionChanged ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// If composition is now on, tell DWM what part of our window should be glass.
	if ( IsCompositionEnabled() )
	{
		MARGINS mar = {0};

		mar.cyBottomHeight = m_cyGlassHeight;
		DwmExtendFrameIntoClientArea ( *this, &mar );
	}

	return 0;
}

void CMainFrame::OnFileExit ( UINT uCode, int nID, HWND hwndCtrl )
{
	PostMessage(WM_CLOSE);
}

void CMainFrame::OnAppAbout ( UINT uCode, int nID, HWND hwndCtrl )
{
	CAboutDlg dlg;

	dlg.DoModal(*this);
}


/////////////////////////////////////////////////////////////////////////////
// Other methods

// Wrapper for DwmIsCompositionEnabled() that's simpler to call.
bool CMainFrame::IsCompositionEnabled() const
{
	HRESULT hr;
	BOOL bEnabled;

	hr = DwmIsCompositionEnabled ( &bEnabled );
	return SUCCEEDED(hr) && bEnabled;
}
