#pragma once
#include "mcx_inject.hpp"

static constexpr uintptr_t va_ErrorHandler = 0x0078def4;

void ErrorHandler(unsigned int err, char* err_msg, char* undefined);

#define STOP(err, fmt, ...) \
	char err_msg[164]; \
	sprintf(err_msg, fmt, __VA_ARGS__); \
	ErrorHandler(err, err_msg, nullptr);