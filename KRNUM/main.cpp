#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <memory>
#include <string_view>
#include <vector>
#include <thread>

#include "definitions.h"
#include "globals.h"
#include "gtautils.h"
#include "hook.h"
#include "math.h"
#include "memory.h"
#include "gui.h"
#include "hacks.h"

#define concat(first, second) first second

uintptr_t base_address = 0;

struct HandleDisposer
{
	using pointer = HANDLE;
	void operator()(HANDLE handle) const
	{
		if (handle != NULL || handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
		}
	}
};

using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;

static std::uint32_t get_process_id(std::string_view process_name)
{
	PROCESSENTRY32 processentry;
	const unique_handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));

	if (snapshot_handle.get() == INVALID_HANDLE_VALUE)
		return NULL;

	processentry.dwSize = sizeof(MODULEENTRY32);

	while (Process32Next(snapshot_handle.get(), &processentry) == TRUE)
	{
		if (process_name.compare(processentry.szExeFile) == NULL)
		{
			return processentry.th32ProcessID;
		}
	}
	return NULL;
}

static ULONG64 get_module_base_address(const char* module_name)
{
	NULL_MEMORY instructions = { 0 };
	instructions.pid = globals::process_id;
	instructions.req_base = TRUE;
	instructions.read = FALSE;
	instructions.write = FALSE;
	instructions.draw_box = FALSE;
	instructions.module_name = module_name;
	call_hook(&instructions);

	ULONG64 base = NULL;
	base = instructions.base_address;
	return base;
}

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE previousInstance, PWSTR arguments, int commandShow)
{
	globals::process_id = get_process_id("gta_sa.exe");
	if (globals::process_id == 0)
	{
		Sleep(5000);
		return EXIT_FAILURE;
	}

	base_address = get_module_base_address("gta_sa.exe");
	if (!base_address)
	{
		Sleep(5000);
		return EXIT_FAILURE;
	}

	std::thread(hacks::VisualsThread).detach();

	//create gui
	gui::CreateHWindow("Aoadkoekfoqo");
	gui::CreateDevice();
	gui::CreateImGui();

	while (gui::isRunning)
	{
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	//destroy gui
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	return EXIT_SUCCESS;
}