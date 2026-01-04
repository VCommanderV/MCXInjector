#pragma once
#include "mcx_inject.hpp"
#include "mcx/gui/a_system.hpp"
#include <cstdint>

VADDR(va_application, 0x0007aae80);
VADDR(va_currPlanet, 0x00807fe8);

VADDR(va_savePath, 0x007aa828);
VADDR(va_directXPath, 0x007aaf90);
VADDR(va_terrainPath, 0x0078a35c);
VADDR(va_soundPath, 0x0078a328);
VADDR(va_interfacePath, 0x007aa7d4);
VADDR(va_paletteName, 0x007ab030);

VADDR(va_artPath, 0x007ab080);
VADDR(va_fontPath, 0x007aaeec);
VADDR(va_spritePath, 0x007aada0);
VADDR(va_palettePath, 0x007aaf3c);

OBJ_REF(long, g_currPlanet, va_currPlanet);

OBJ_REF(char, g_savePath, va_savePath);
OBJ_REF(char, g_directXPath, va_directXPath);
OBJ_REF(char, g_terrainPath, va_terrainPath);
OBJ_REF(char, g_soundPath, va_soundPath);
OBJ_REF(char, g_interfacePath, va_interfacePath);
OBJ_REF(char, g_paletteName, va_paletteName);

OBJ_REF(char, g_artPath, va_artPath);
OBJ_REF(char, g_fontPath, va_fontPath);
OBJ_REF(char, g_palettePath, va_palettePath);
OBJ_REF(char, g_spritePath, va_spritePath);

PTR_REF(aSystem, g_application, va_application);