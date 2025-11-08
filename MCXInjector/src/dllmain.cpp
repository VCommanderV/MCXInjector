#include <windows.h>
#include <stdio.h>
#include <iostream>
#include "detours.h"
#include "mcx_inject.hpp"

#include "absl/base/log_severity.h"
#include "absl/base/macros.h"
#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"

static void EnsureConsoleAndStdIO()
{
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        if (!AllocConsole()) {
            return;
        }
    }

    // Redirect C std* to the console using secure API
    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    if (fp) setvbuf(stdout, nullptr, _IONBF, 0);

    freopen_s(&fp, "CONOUT$", "w", stderr);
    if (fp) setvbuf(stderr, nullptr, _IONBF, 0);

    freopen_s(&fp, "CONIN$", "r", stdin);
    if (fp) setvbuf(stdin, nullptr, _IONBF, 0);

    // Make cpp iostreams use the C stdio handles
    std::ios::sync_with_stdio(true);
    std::cout.clear();
    std::cerr.clear();
    std::cin.clear();

    SetConsoleTitleA("MCX Injector Console");

    HWND hw = GetConsoleWindow();
    if (hw) {
        ShowWindow(hw, SW_SHOW);
        SetForegroundWindow(hw);
    }

    absl::SetStderrThreshold(absl::LogSeverityAtLeast::kInfo);
    absl::InitializeLog();
    LOG_DEBUG << "Initialized logging system";
}

static DWORD WINAPI ThreadAttach(LPVOID)
{
    EnsureConsoleAndStdIO();
    PatchRegistry::instance()->ApplyAll();
    return 0;
}

extern "C" __declspec(dllexport) void DetoursInit()
{
    // Note: The new process will fail to start if the target DLL does not contain a exported function with ordinal #1.
    // WORKAROUND: intentionally empty - exists so Detours/launcher can find ordinal #1
}

static BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
    (void)hinst; (void)reserved;

    if (DetourIsHelperProcess()) {
        return TRUE;
    }

    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinst);
        // start attach in a new thread
        HANDLE h = CreateThread(NULL, 0, ThreadAttach, NULL, 0, NULL);
        if (h) CloseHandle(h);
    }
    else if (reason == DLL_PROCESS_DETACH) {
        PatchRegistry::instance()->RestoreAll();
    }

    return TRUE;
}
