#include <windows.h>
#include <stdio.h>
#include <detours.h>
#include <WinBase.h>
#include <cstdlib>
#include <filesystem>

static auto constexpr MCXI_VERSION = "0.0.1";

int main(int argc, char** argv)
{
    char* value = nullptr;
    size_t len = 0;

    errno_t err = _dupenv_s(&value, &len, "MCXI_EXE_PATH");

    if (err != 0 || value == nullptr) {
        printf("MCXI_EXE_PATH env variable not set. bail.");
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

    printf("MCXInjector v%s\n\n", MCXI_VERSION);
    printf("Target EXE = %s\n", target_exe);
    printf("Workdir = %s\n", work_dir);
    printf("Injector DLL = %s\n", hook_dll);
  
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
        CREATE_NEW_CONSOLE | CREATE_DEFAULT_ERROR_MODE,
        NULL,
        work_dir, // working directory, if not set to the folder where the exe is, the process crashes
        &si,
        &pi,
        hook_dll,
        NULL
    );

    if (!ok) {
        printf("Failed to start process with Detour DLL. Error %lu\n", GetLastError());
        return 1;
    }

    printf("Started process %lu with hook DLL %s\n", pi.dwProcessId, hook_dll);

    ResumeThread(pi.hThread);

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD result = 0;
    if (!GetExitCodeProcess(pi.hProcess, &result)) {
        printf("withdll.exe: GetExitCodeProcess failed: %ld\n", GetLastError());
        return 9010;
    }

    free(value);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}
