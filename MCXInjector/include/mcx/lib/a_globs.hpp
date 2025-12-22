#pragma once
#include "mcx_inject.hpp"
#include "mcx/gui/a_object.hpp"
#include <cstdint>

static constexpr uintptr_t va_currPlanet = 0x00807fe8;
static constexpr uintptr_t va_artPath = 0x007ab080;
static constexpr uintptr_t va_application = 0x0007aae80;

OBJ_REF(long, g_currPlanet, va_currPlanet);
OBJ_REF(char, g_artPath, va_artPath);
PTR_REF(aObject, g_application, va_application); // TODO: it is aSystem actually