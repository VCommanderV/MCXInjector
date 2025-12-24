#pragma once
#include "mcx_inject.hpp"
#include "mcx/gui/a_system.hpp"
#include <cstdint>

VADDR(va_currPlanet, 0x00807fe8);
VADDR(va_artPath, 0x007ab080);
VADDR(va_application, 0x0007aae80);

OBJ_REF(long, g_currPlanet, va_currPlanet);
OBJ_REF(char, g_artPath, va_artPath);
PTR_REF(aSystem, g_application, va_application); // TODO: it is aSystem actually