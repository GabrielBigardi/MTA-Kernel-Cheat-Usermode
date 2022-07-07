#pragma once
#include "Windows.h"
#include <cstdint>

//template<typename ... Arg>
//uint64_t call_hook(const Arg ...);

template<typename ... Arg>
uint64_t call_hook(const Arg ... args)
{
	LoadLibrary("user32.dll");
	void* hooked_func = GetProcAddress(LoadLibrary("win32u.dll"), "NtDxgkGetAvailableTrackedWorkloadIndex");

	auto func = static_cast<uint64_t(_stdcall*)(Arg...)>(hooked_func);

	return func(args ...);
}