#include "stdafx.h"
#include "ProcessMemoryViewer.h"
#include "ProcessMemoryViewerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CProcessMemoryViewerDlg dialog

CProcessMemoryViewerDlg::CProcessMemoryViewerDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CProcessMemoryViewerDlg::IDD, pParent)
    , m_selectedProcessId(0)
    , m_hSelectedProcess(NULL)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessMemoryViewerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROCESS_COMBO, m_processCombo);
    DDX_Control(pDX, IDC_MEMORY_LIST, m_memoryList);
    DDX_Control(pDX, IDC_REFRESH_BTN, m_refreshBtn);
    DDX_Control(pDX, IDC_OPEN_PROCESS_BTN, m_openProcessBtn);
    DDX_Control(pDX, IDC_TOTAL_MEMORY_STATIC, m_totalMemoryStatic);
    DDX_Control(pDX, IDC_PRIVATE_MEMORY_STATIC, m_privateMemoryStatic);
    DDX_Control(pDX, IDC_MAPPED_MEMORY_STATIC, m_mappedMemoryStatic);
    DDX_Control(pDX, IDC_IMAGE_MEMORY_STATIC, m_imageMemoryStatic);
    DDX_Control(pDX, IDC_PROCESS_INFO_STATIC, m_processInfoStatic);
}

BEGIN_MESSAGE_MAP(CProcessMemoryViewerDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_REFRESH_BTN, &CProcessMemoryViewerDlg::OnBnClickedRefreshBtn)
    ON_BN_CLICKED(IDC_OPEN_PROCESS_BTN, &CProcessMemoryViewerDlg::OnBnClickedOpenProcessBtn)
    ON_CBN_SELCHANGE(IDC_PROCESS_COMBO, &CProcessMemoryViewerDlg::OnCbnSelchangeProcessCombo)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// CProcessMemoryViewerDlg message handlers

BOOL CProcessMemoryViewerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // Initialize the memory list control with columns
    m_memoryList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_memoryList.InsertColumn(0, _T("Base Address"), LVCFMT_LEFT, 120);
    m_memoryList.InsertColumn(1, _T("Size"), LVCFMT_RIGHT, 100);
    m_memoryList.InsertColumn(2, _T("State"), LVCFMT_LEFT, 80);
    m_memoryList.InsertColumn(3, _T("Protection"), LVCFMT_LEFT, 150);
    m_memoryList.InsertColumn(4, _T("Type"), LVCFMT_LEFT, 100);
    m_memoryList.InsertColumn(5, _T("End Address"), LVCFMT_LEFT, 120);

    // Enable debug privilege to access all processes
    EnableDebugPrivilege();

    // Populate process list
    EnumerateProcesses();

    return TRUE;
}

void CProcessMemoryViewerDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CProcessMemoryViewerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

BOOL CProcessMemoryViewerDlg::EnableDebugPrivilege()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return FALSE;

    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid))
    {
        CloseHandle(hToken);
        return FALSE;
    }

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, NULL);

    BOOL result = (GetLastError() == ERROR_SUCCESS);
    CloseHandle(hToken);
    return result;
}

void CProcessMemoryViewerDlg::EnumerateProcesses()
{
    m_processes.clear();
    m_processCombo.ResetContent();

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox(_T("Failed to create process snapshot"));
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32))
    {
        do
        {
            ProcessInfo info;
            info.dwProcessId = pe32.th32ProcessID;
            info.strProcessName = pe32.szExeFile;
            info.strDisplayName.Format(_T("%s (PID: %d)"), pe32.szExeFile, pe32.th32ProcessID);

            m_processes.push_back(info);
            m_processCombo.AddString(info.strDisplayName);

        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);

    if (m_processCombo.GetCount() > 0)
    {
        m_processCombo.SetCurSel(0);
    }
}

void CProcessMemoryViewerDlg::EnumerateMemoryRegions(DWORD processId)
{
    m_memoryRegions.clear();

    if (m_hSelectedProcess != NULL)
    {
        CloseHandle(m_hSelectedProcess);
        m_hSelectedProcess = NULL;
    }

    m_hSelectedProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (m_hSelectedProcess == NULL)
    {
        CString msg;
        msg.Format(_T("Failed to open process %d. Error: %d\nTry running as Administrator."),
                   processId, GetLastError());
        AfxMessageBox(msg);
        return;
    }

    MEMORY_BASIC_INFORMATION mbi;
    PVOID address = NULL;
    SIZE_T result;

    while (TRUE)
    {
        result = VirtualQueryEx(m_hSelectedProcess, address, &mbi, sizeof(mbi));
        if (result == 0)
            break;

        // Only add regions that are committed
        if (mbi.State == MEM_COMMIT || mbi.State == MEM_RESERVE || mbi.State == MEM_FREE)
        {
            MemoryRegionInfo regionInfo;
            regionInfo.baseAddress = mbi.BaseAddress;
            regionInfo.regionSize = mbi.RegionSize;
            regionInfo.state = mbi.State;
            regionInfo.protect = mbi.Protect;
            regionInfo.type = mbi.Type;
            regionInfo.strState = GetStateString(mbi.State);
            regionInfo.strProtect = GetProtectionString(mbi.Protect);
            regionInfo.strType = GetTypeString(mbi.Type);

            m_memoryRegions.push_back(regionInfo);
        }

        address = (PVOID)((SIZE_T)mbi.BaseAddress + mbi.RegionSize);
    }

    UpdateMemoryList();
    UpdateStatistics();
}

void CProcessMemoryViewerDlg::UpdateMemoryList()
{
    m_memoryList.DeleteAllItems();

    for (size_t i = 0; i < m_memoryRegions.size(); i++)
    {
        const MemoryRegionInfo& info = m_memoryRegions[i];

        CString strBaseAddress;
        strBaseAddress.Format(_T("0x%016IX"), (SIZE_T)info.baseAddress);

        int nItem = m_memoryList.InsertItem(static_cast<int>(i), strBaseAddress);
        m_memoryList.SetItemText(nItem, 1, FormatMemorySize(info.regionSize));
        m_memoryList.SetItemText(nItem, 2, info.strState);
        m_memoryList.SetItemText(nItem, 3, info.strProtect);
        m_memoryList.SetItemText(nItem, 4, info.strType);

        CString strEndAddress;
        strEndAddress.Format(_T("0x%016IX"), (SIZE_T)info.baseAddress + info.regionSize);
        m_memoryList.SetItemText(nItem, 5, strEndAddress);
    }
}

void CProcessMemoryViewerDlg::UpdateStatistics()
{
    SIZE_T totalMemory = 0;
    SIZE_T privateMemory = 0;
    SIZE_T mappedMemory = 0;
    SIZE_T imageMemory = 0;

    for (const auto& region : m_memoryRegions)
    {
        if (region.state == MEM_COMMIT)
        {
            totalMemory += region.regionSize;

            if (region.type == MEM_PRIVATE)
                privateMemory += region.regionSize;
            else if (region.type == MEM_MAPPED)
                mappedMemory += region.regionSize;
            else if (region.type == MEM_IMAGE)
                imageMemory += region.regionSize;
        }
    }

    CString strTotal, strPrivate, strMapped, strImage;
    strTotal.Format(_T("Total Committed: %s"), FormatMemorySize(totalMemory));
    strPrivate.Format(_T("Private: %s"), FormatMemorySize(privateMemory));
    strMapped.Format(_T("Mapped: %s"), FormatMemorySize(mappedMemory));
    strImage.Format(_T("Image: %s"), FormatMemorySize(imageMemory));

    m_totalMemoryStatic.SetWindowText(strTotal);
    m_privateMemoryStatic.SetWindowText(strPrivate);
    m_mappedMemoryStatic.SetWindowText(strMapped);
    m_imageMemoryStatic.SetWindowText(strImage);

    // Update process info
    CString strProcessInfo;
    strProcessInfo.Format(_T("Process ID: %d | Memory Regions: %zu"),
                          m_selectedProcessId, m_memoryRegions.size());
    m_processInfoStatic.SetWindowText(strProcessInfo);
}

CString CProcessMemoryViewerDlg::FormatMemorySize(SIZE_T size)
{
    CString result;
    const SIZE_T KB = 1024;
    const SIZE_T MB = 1024 * KB;
    const SIZE_T GB = 1024 * MB;

    if (size >= GB)
        result.Format(_T("%.2f GB"), (double)size / GB);
    else if (size >= MB)
        result.Format(_T("%.2f MB"), (double)size / MB);
    else if (size >= KB)
        result.Format(_T("%.2f KB"), (double)size / KB);
    else
        result.Format(_T("%zu bytes"), size);

    return result;
}

CString CProcessMemoryViewerDlg::GetStateString(DWORD state)
{
    switch (state)
    {
    case MEM_COMMIT:
        return _T("COMMIT");
    case MEM_RESERVE:
        return _T("RESERVE");
    case MEM_FREE:
        return _T("FREE");
    default:
        return _T("UNKNOWN");
    }
}

CString CProcessMemoryViewerDlg::GetProtectionString(DWORD protect)
{
    CString result;

    if (protect & PAGE_NOACCESS)
        result += _T("NOACCESS ");
    if (protect & PAGE_READONLY)
        result += _T("READONLY ");
    if (protect & PAGE_READWRITE)
        result += _T("READWRITE ");
    if (protect & PAGE_WRITECOPY)
        result += _T("WRITECOPY ");
    if (protect & PAGE_EXECUTE)
        result += _T("EXECUTE ");
    if (protect & PAGE_EXECUTE_READ)
        result += _T("EXECUTE_READ ");
    if (protect & PAGE_EXECUTE_READWRITE)
        result += _T("EXECUTE_READWRITE ");
    if (protect & PAGE_EXECUTE_WRITECOPY)
        result += _T("EXECUTE_WRITECOPY ");
    if (protect & PAGE_GUARD)
        result += _T("| GUARD ");
    if (protect & PAGE_NOCACHE)
        result += _T("| NOCACHE ");
    if (protect & PAGE_WRITECOMBINE)
        result += _T("| WRITECOMBINE ");

    if (result.IsEmpty())
        result = _T("NONE");

    result.Trim();
    return result;
}

CString CProcessMemoryViewerDlg::GetTypeString(DWORD type)
{
    switch (type)
    {
    case MEM_IMAGE:
        return _T("IMAGE");
    case MEM_MAPPED:
        return _T("MAPPED");
    case MEM_PRIVATE:
        return _T("PRIVATE");
    default:
        return _T("-");
    }
}

void CProcessMemoryViewerDlg::OnBnClickedRefreshBtn()
{
    EnumerateProcesses();
}

void CProcessMemoryViewerDlg::OnBnClickedOpenProcessBtn()
{
    int nIndex = m_processCombo.GetCurSel();
    if (nIndex == CB_ERR || nIndex >= static_cast<int>(m_processes.size()))
    {
        AfxMessageBox(_T("Please select a process first"));
        return;
    }

    m_selectedProcessId = m_processes[nIndex].dwProcessId;
    EnumerateMemoryRegions(m_selectedProcessId);
}

void CProcessMemoryViewerDlg::OnCbnSelchangeProcessCombo()
{
    int nIndex = m_processCombo.GetCurSel();
    if (nIndex != CB_ERR && nIndex < static_cast<int>(m_processes.size()))
    {
        m_selectedProcessId = m_processes[nIndex].dwProcessId;
        EnumerateMemoryRegions(m_selectedProcessId);
    }
}

void CProcessMemoryViewerDlg::OnDestroy()
{
    if (m_hSelectedProcess != NULL)
    {
        CloseHandle(m_hSelectedProcess);
        m_hSelectedProcess = NULL;
    }

    CDialogEx::OnDestroy();
}
