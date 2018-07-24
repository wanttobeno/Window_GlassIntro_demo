// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "aboutdlg.h"

LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    m_wndLink.SetHyperLinkExtendedStyle ( HLINK_USETAGS );
    m_wndLink.SetHyperLink ( _T("http://www.codeproject.com/script/articles/list_articles.asp?userid=152") );
    m_wndLink.SubclassWindow ( GetDlgItem(IDC_ARTICLES_LINK) );

    CenterWindow(GetParent());
    return TRUE;
}

LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    EndDialog(wID);
    return 0;
}
