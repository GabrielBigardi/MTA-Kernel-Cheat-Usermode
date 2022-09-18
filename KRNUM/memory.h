#pragma once
#include <Windows.h>
#include "definitions.h"
#include "hook.h"
#include "globals.h"


bool Write_memory(UINT_PTR write_address, UINT_PTR source_address, SIZE_T write_size);

template <class T>
T Read(UINT_PTR read_address)
{
	T response{};
	NULL_MEMORY instructions;
	instructions.pid = globals::csgo_process_id;
	instructions.size = sizeof(T);
	instructions.address = read_address;
	instructions.read = TRUE;
	instructions.write = FALSE;
	instructions.req_base = FALSE;
	instructions.draw_box = FALSE;
	instructions.output = &response;
	call_hook(&instructions);

	return response;
}

template<typename S>
bool Write(UINT_PTR write_address, const S& value)
{
	return Write_memory(write_address, (UINT_PTR)&value, sizeof(S));
}