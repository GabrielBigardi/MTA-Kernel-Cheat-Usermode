//#include <Windows.h>
//#include "hook.h"
//#include "definitions.h"

//extern ULONG process_id;

//template <class T>
//T Read(UINT_PTR read_address)
//{
//	T response{};
//	NULL_MEMORY instructions;
//	instructions.pid = process_id;
//	instructions.size = sizeof(T);
//	instructions.address = read_address;
//	instructions.read = TRUE;
//	instructions.write = FALSE;
//	instructions.req_base = FALSE;
//	instructions.draw_box = FALSE;
//	instructions.output = &response;
//	call_hook(&instructions);
//
//	return response;
//}
//
//template<typename S>
//bool write(UINT_PTR write_address, const S& value)
//{
//	return write_memory(write_address, (UINT_PTR)&value, sizeof(S));
//}

//bool write_memory(UINT_PTR write_address, UINT_PTR source_address, SIZE_T write_size)
//{
//	NULL_MEMORY instructions;
//	instructions.address = write_address;
//	instructions.pid = process_id;
//	instructions.write = TRUE;
//	instructions.read = FALSE;
//	instructions.req_base = FALSE;
//	instructions.draw_box = FALSE;
//	instructions.buffer_address = (void*)source_address;
//	instructions.size = write_size;
//
//	call_hook(&instructions);
//
//	return true;
//}