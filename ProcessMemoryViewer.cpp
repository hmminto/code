#include "stdafx.h"
#include "ProcessMemoryViewer.h"
#include "ProcessMemoryViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CProcessMemoryViewerApp

BEGIN_MESSAGE_MAP(CProcessMemoryViewerApp, CWinApp)
END_MESSAGE_MAP()

// CProcessMemoryViewerApp construction

CProcessMemoryViewerApp::CProcessMemoryViewerApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

// The one and only CProcessMemoryViewerApp object

CProcessMemoryViewerApp theApp;

// CProcessMemoryViewerApp initialization

BOOL CProcessMemoryViewerApp::InitInstance()
{
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    // Create and show the main dialog
    CProcessMemoryViewerDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();

    // Since the dialog has been closed, return FALSE so that we exit the
    // application, rather than start the application's message pump.
    return FALSE;
}
