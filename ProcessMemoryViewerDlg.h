#pragma once
#include "afxcmn.h"
#include <vector>
#include <TlHelp32.h>
#include <Psapi.h>

// Structure to hold process information
typedef struct _ProcessInfo
{
    DWORD dwProcessId;
    CString strProcessName;
    CString strDisplayName;
} ProcessInfo;

// Structure to hold memory region information
typedef struct _MEMORYREGIONINFO
{
    PVOID baseAddress;
    SIZE_T regionSize;
    DWORD state;
    DWORD protect;
    DWORD type;
    CString strState;
    CString strProtect;
    CString strType;
} MEMORYREGIONINFO;

// CProcessMemoryViewerDlg dialog
class CProcessMemoryViewerDlg : public CDialogEx
{
// Construction
public:
    CProcessMemoryViewerDlg(CWnd* pParent = NULL);

// Dialog Data
    enum { IDD = IDD_PROCESSMEMORYVIEWER_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

private:
    // Controls
    CComboBox m_processCombo;
    CListCtrl m_memoryList;
    CButton m_refreshBtn;
    CButton m_openProcessBtn;
    CStatic m_totalMemoryStatic;
    CStatic m_privateMemoryStatic;
    CStatic m_mappedMemoryStatic;
    CStatic m_imageMemoryStatic;
    CStatic m_processInfoStatic;

    // Data
    std::vector<ProcessInfo> m_processes;
    std::vector<MEMORYREGIONINFO> m_memoryRegions;
    DWORD m_selectedProcessId;
    HANDLE m_hSelectedProcess;

    // Helper functions
    void EnumerateProcesses();
    void EnumerateMemoryRegions(DWORD processId);
    void UpdateMemoryList();
    void UpdateStatistics();
    CString FormatMemorySize(SIZE_T size);
    CString GetStateString(DWORD state);
    CString GetProtectionString(DWORD protect);
    CString GetTypeString(DWORD type);
    BOOL EnableDebugPrivilege();

    // Message handlers
    afx_msg void OnBnClickedRefreshBtn();
    afx_msg void OnBnClickedOpenProcessBtn();
    afx_msg void OnCbnSelchangeProcessCombo();
    afx_msg void OnDestroy();
};
