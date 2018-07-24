// GlassClockDlg.cpp : main source file for GlassClockDlg.exe
//

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#pragma comment(lib,"gdiplus")

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;

	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if ( dlgMain.Create(NULL) == NULL )
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = CoInitialize(NULL);

	ATLASSERT(SUCCEEDED(hRes));

	AtlInitCommonControls(ICC_WIN95_CLASSES);

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	GdiplusStartupInput gsi;
	ULONG_PTR uToken;

	GdiplusStartup ( &uToken, &gsi, NULL );

	int nRet = Run(lpstrCmdLine, nCmdShow);

	GdiplusShutdown ( uToken );
	_Module.Term();
	CoUninitialize();
	return nRet;
}
