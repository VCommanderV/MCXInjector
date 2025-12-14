#pragma once
#include "mcx_inject.hpp"
#include "mcx/gui/a_object.hpp"
#include <cstdint>

static constexpr uintptr_t va_currPlanet = 0x00807fe8;
static constexpr uintptr_t va_artPath = 0x007ab080;
static constexpr uintptr_t va_application = 0x0007aae80;


static long* g_currPlanet = reinterpret_cast<long*>(MCX_RUNTIME_ADDR(va_currPlanet));
static char* g_artPath = reinterpret_cast<char*>(MCX_RUNTIME_ADDR(va_artPath));
// TODO: it is aSystem actually
static aObject** g_application = reinterpret_cast<aObject**>(MCX_RUNTIME_ADDR(va_application));

namespace mcx
{
	static long GetCurrPlanet()
	{
		return *g_currPlanet;
	}

	static char* GetArtPath()
	{
		return g_artPath;
	}

	static aObject* GetApplication()
	{
		return *g_application;
	}
}