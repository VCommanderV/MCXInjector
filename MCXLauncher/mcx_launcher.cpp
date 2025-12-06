#include <windows.h>
#include <stdio.h>
#include <WinBase.h>
#include <cstdlib>
#include <filesystem>
#include <detours.h>

#include "absl/base/log_severity.h"
#include "absl/base/macros.h"
#include "absl/log/globals.h"
#include "absl/flags/parse.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"

static auto constexpr MCXI_VERSION = "0.17.1";

int main(int argc, char** argv)
{   

    absl::SetStderrThreshold(absl::LogSeverityAtLeast::kInfo);
    absl::ParseCommandLine(argc, argv);
    absl::InitializeLog();

    char* value = nullptr;
    size_t len = 0;

    errno_t err = _dupenv_s(&value, &len, "MCXI_EXE_PATH");

    if (err != 0 || value == nullptr) {
        LOG(ERROR) << "MCXI_EXE_PATH env variable not set. bail.";
        return 1;
    }
    
    const char* target_exe = value;

    std::filesystem::path filePath(target_exe);
    auto parent_path = filePath.parent_path().string();
    const char* work_dir = parent_path.c_str();

    // Get current executing exe path
    char buffer[MAX_PATH];
    DWORD len2 = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    auto cwd = std::filesystem::path(std::string(buffer, len2)).parent_path();

    auto hook_dll_path = (cwd / "MCXInjector.dll").string();
    const char* hook_dll = hook_dll_path.c_str();

    LOG(INFO) << "MCXInjector v" << MCXI_VERSION;
    LOG(INFO) << "Target EXE = " << target_exe;
    LOG(INFO) << "Workdir = " << work_dir;
    LOG(INFO) << "Injector DLL = " << hook_dll;
  
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    BOOL ok = DetourCreateProcessWithDllExA(
        target_exe,
        NULL, // command line args
        NULL, NULL,
        FALSE,
        CREATE_NEW_CONSOLE | CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED,
        NULL,
        work_dir, // working directory, if not set to the folder where the exe is, the process crashes
        &si,
        &pi,
        hook_dll,
        NULL
    );

    if (!ok) {
        LOG(ERROR) << "Failed to start process with Detour DLL. Error " << GetLastError();
        return 1;
    }

    LOG(INFO) << "Started target process PID=" << pi.dwProcessId;

    ResumeThread(pi.hThread);

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD result = 0;
    if (!GetExitCodeProcess(pi.hProcess, &result)) {
        LOG(ERROR) << "withdll.exe: GetExitCodeProcess failed: " << GetLastError();
        return 9010;
    }

    free(value);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}
