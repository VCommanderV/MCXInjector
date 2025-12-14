#pragma once
#include <cstdint>
#include "mcx_inject.hpp"
#include "mcx/lib/vfx.hpp"

// long __cdecl DrawTransparent(struct _pane *,struct _window *,int,int,int,int)
long __cdecl DrawTransparent(_pane*,_window*, int, int, int, int);

#pragma region VAs
static constexpr uintptr_t va_vfx_DrawTransparent = 0x006b6f20;
#pragma endregion

#pragma region FuncPtrs
static auto _DrawTransparent_PTR = reinterpret_cast<long(__cdecl*)(_pane*, _window*, int, int, int, int)>(MCX_RUNTIME_ADDR(va_vfx_DrawTransparent));
#pragma endregion