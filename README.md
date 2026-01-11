# Process Memory Viewer

A Windows desktop application built with MFC (Microsoft Foundation Classes) that provides detailed visualization of process memory layout and usage.

## Features

### Process Management
- **Process Enumeration**: Lists all running processes on the system
- **Process Selection**: Easy dropdown selection with Process ID display
- **Real-time Refresh**: Update process list on demand

### Memory Visualization
- **Memory Region Display**: Shows all memory regions for the selected process
- **Detailed Information**: For each memory region:
  - Base Address (64-bit hex format)
  - Region Size (formatted as bytes/KB/MB/GB)
  - Memory State (COMMIT, RESERVE, FREE)
  - Protection Flags (READONLY, READWRITE, EXECUTE, etc.)
  - Memory Type (IMAGE, MAPPED, PRIVATE)
  - End Address

### Memory Statistics
- **Total Committed Memory**: Sum of all committed memory regions
- **Private Memory**: Memory allocated by the process itself
- **Mapped Memory**: Memory-mapped files
- **Image Memory**: Executable code and DLLs

### Advanced Features
- **Debug Privilege Elevation**: Automatically requests SE_DEBUG_NAME privilege to access all processes
- **64-bit Support**: Full support for 64-bit address space visualization
- **Grid View**: Sortable list control with full row selection and grid lines

## Technical Details

### Memory Enumeration
The application uses the Windows API function `VirtualQueryEx` to enumerate memory regions:
- Iterates through the entire address space of the target process
- Queries memory information using `MEMORY_BASIC_INFORMATION` structure
- Displays all memory states including committed, reserved, and free regions

### Protection Flags
Displays comprehensive protection information:
- `NOACCESS`: No access allowed
- `READONLY`: Read-only access
- `READWRITE`: Read and write access
- `EXECUTE`: Execute only
- `EXECUTE_READ`: Execute and read
- `EXECUTE_READWRITE`: Execute, read, and write
- `GUARD`: Guard page
- `NOCACHE`: No cache
- `WRITECOMBINE`: Write combine

### Memory Types
- **IMAGE**: Memory used for executable code (EXE, DLL)
- **MAPPED**: Memory-mapped files
- **PRIVATE**: Private memory allocated by the process

## Building the Application

### Prerequisites
- **Windows Operating System**: Windows 7 or later (Windows 10/11 recommended)
- **Visual Studio**: Visual Studio 2019 or later with:
  - Desktop development with C++ workload
  - MFC and ATL support (v142 or later)
  - Windows 10 SDK

### Build Steps

1. **Clone the Repository**
   ```bash
   git clone <repository-url>
   cd ProcessMemoryViewer
   ```

2. **Create Icon File** (if not present)
   - See ICON_NOTE.txt for instructions
   - Create or copy a .ico file named `ProcessMemoryViewer.ico` to the project root

3. **Open Solution**
   - Open `ProcessMemoryViewer.sln` in Visual Studio

4. **Select Build Configuration**
   - Debug or Release
   - x86 or x64 (x64 recommended for viewing 64-bit processes)

5. **Build**
   - Press F7 or Build -> Build Solution
   - The executable will be created in the Debug or Release folder

### Build Configurations
- **Debug|Win32**: 32-bit debug build
- **Debug|x64**: 64-bit debug build (recommended for development)
- **Release|Win32**: 32-bit optimized build
- **Release|x64**: 64-bit optimized build (recommended for production)

## Usage

### Running the Application

1. **Launch**: Run `ProcessMemoryViewer.exe`
2. **Administrator Rights**: For best results, run as Administrator to access all processes
3. **Select Process**: Choose a process from the dropdown list
4. **Click "Open Process"**: Or the selection will auto-update the memory view
5. **View Memory Layout**: Browse memory regions in the list control

### Tips
- **Run as Administrator**: Required to view memory of system processes
- **Use 64-bit Build**: To properly view 64-bit processes on 64-bit Windows
- **Refresh Process List**: Click "Refresh List" to update available processes
- **Sort Columns**: Click column headers to sort memory regions

## Architecture

### Class Structure

#### `CProcessMemoryViewerApp`
- Main application class
- Inherits from `CWinApp`
- Handles application initialization

#### `CProcessMemoryViewerDlg`
- Main dialog class
- Inherits from `CDialogEx`
- Manages UI and process/memory enumeration

#### Data Structures

**ProcessInfo**
```cpp
struct ProcessInfo {
    DWORD dwProcessId;        // Process ID
    CString strProcessName;   // Process executable name
    CString strDisplayName;   // Display name (Name + PID)
};
```

**MemoryRegionInfo**
```cpp
struct MemoryRegionInfo {
    PVOID baseAddress;        // Base address of region
    SIZE_T regionSize;        // Size in bytes
    DWORD state;              // MEM_COMMIT, MEM_RESERVE, MEM_FREE
    DWORD protect;            // Protection flags
    DWORD type;               // MEM_IMAGE, MEM_MAPPED, MEM_PRIVATE
    CString strState;         // Formatted state string
    CString strProtect;       // Formatted protection string
    CString strType;          // Formatted type string
};
```

### Key Methods

- `EnumerateProcesses()`: Lists all running processes using `CreateToolhelp32Snapshot`
- `EnumerateMemoryRegions()`: Queries memory layout using `VirtualQueryEx`
- `UpdateMemoryList()`: Refreshes the list control display
- `UpdateStatistics()`: Calculates and displays memory statistics
- `EnableDebugPrivilege()`: Requests SE_DEBUG_NAME privilege

## Security Considerations

- **Privilege Escalation**: The application requests debug privileges to access all processes
- **Read-Only Access**: The application only reads memory information, it does not modify memory
- **Process Handle Management**: Properly opens and closes process handles to prevent resource leaks

## Windows API Functions Used

- `CreateToolhelp32Snapshot`: Enumerate processes
- `Process32First/Process32Next`: Iterate through processes
- `OpenProcess`: Open process with QUERY_INFORMATION and VM_READ access
- `VirtualQueryEx`: Query memory information
- `OpenProcessToken`: Access current process token
- `LookupPrivilegeValue`: Get privilege LUID
- `AdjustTokenPrivileges`: Enable debug privilege

## Limitations

- **32-bit Process Limitation**: A 32-bit build cannot fully access 64-bit process memory
- **Access Denied**: Some system processes may be inaccessible even with debug privilege
- **Protected Processes**: Windows Protected Processes (like some antivirus services) cannot be accessed

## Future Enhancements

Potential improvements:
- Memory dump functionality
- Memory search capabilities
- Real-time memory monitoring
- Memory change detection
- Export memory layout to file
- Enhanced visualization (graphs, charts)
- Memory pattern analysis

## Troubleshooting

### "Failed to open process" Error
- **Solution**: Run the application as Administrator
- **Cause**: Insufficient privileges to access the target process

### Cannot See 64-bit Processes Properly
- **Solution**: Use the x64 build configuration
- **Cause**: 32-bit processes have limited access to 64-bit address space

### Empty Memory List
- **Solution**: Ensure the process is still running and accessible
- **Cause**: Process may have terminated or requires higher privileges

## License

This project is provided as-is for educational and development purposes.

## Contributing

Contributions are welcome! Please ensure:
- Code follows MFC conventions
- Proper error handling is implemented
- Changes are tested on both x86 and x64 builds
- Documentation is updated accordingly

## System Requirements

- **OS**: Windows 7 SP1 or later
- **Architecture**: x86 or x64
- **Dependencies**: Visual C++ Redistributable (included with build)
- **Privileges**: Administrator rights recommended

## Version History

### Version 1.0.0
- Initial release
- Process enumeration
- Memory region visualization
- Memory statistics
- Debug privilege support
- 64-bit address space support
