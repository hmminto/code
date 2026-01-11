#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

// CProcessMemoryViewerApp:
// See ProcessMemoryViewer.cpp for the implementation of this class
//

class CProcessMemoryViewerApp : public CWinApp
{
public:
    CProcessMemoryViewerApp();

// Overrides
public:
    virtual BOOL InitInstance();

// Implementation
    DECLARE_MESSAGE_MAP()
};

extern CProcessMemoryViewerApp theApp;
