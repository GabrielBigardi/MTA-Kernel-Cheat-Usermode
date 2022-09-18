#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <memory>
#include <string_view>
#include <vector>

#include "definitions.h"
#include "globals.h"
#include "gtautils.h"
#include "hook.h"
#include "math.h"
#include "memory.h"
#include "draw.h"



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

static ULONG64 get_module_base_address(uintptr_t pid, const char* module_name)
{
	NULL_MEMORY instructions = { 0 };
	instructions.pid = pid;
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

static ULONG64 get_module_base_address_x86(uintptr_t pid, const char* module_name)
{
	NULL_MEMORY instructions = { 0 };
	instructions.pid = pid;
	instructions.req_base_x86 = TRUE;
	instructions.read = FALSE;
	instructions.write = FALSE;
	instructions.draw_box = FALSE;
	instructions.module_name = module_name;
	call_hook(&instructions);

	ULONG64 base = NULL;
	base = instructions.base_address;
	return base;
}

HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

int main()
{
	SetConsoleTextAttribute(hConsole, 6);
//	std::cout << R"(
//:::    ::: ::::::::::: :::    :::       :::     :::   :::        :::::::  
//:+:    :+:     :+:     :+:    :+:       :+:     :+: :+:+:       :+:   :+: 
//+:+    +:+     +:+      +:+  +:+        +:+     +:+   +:+       +:+  :+:+ 
//+#++:++#++     +#+       +#++:+         +#+     +:+   +#+       +#+ + +:+ 
//+#+    +#+     +#+      +#+  +#+         +#+   +#+    +#+       +#+#  +#+ 
//#+#    #+#     #+#     #+#    #+#         #+#+#+#     #+#   #+# #+#   #+# 
//###    ###     ###     ###    ###           ###     ####### ###  #######  
//)" << std::endl;
	std::cout << R"(
88      888888888888                          88         ,a8888a,     
88           88                             ,d88       ,8P"'  `"Y8,   
88           88                           888888      ,8P        Y8,  
88,dPPYba,   88  8b,     ,d8     8b       d8  88      88          88  
88P'    "8a  88   `Y8, ,8P'      `8b     d8'  88      88          88  
88       88  88     )888(         `8b   d8'   88      `8b        d8'  
88       88  88   ,d8" "8b,        `8b,d8'    88  888  `8ba,  ,ad8'   
88       88  88  8P'     `Y8         "8"      88  888    "Y8888P"     
)" << std::endl;
	SetConsoleTextAttribute(hConsole, 7);
	std::cout << "Trying to locate processes... " << std::endl;
	globals::csgo_process_id = get_process_id("csgo.exe");

	std::cout << "csgo.exe: ";

	if (globals::csgo_process_id == 0) {
		SetConsoleTextAttribute(hConsole, 4);
		std::cout << "NOT FOUND" << std::endl;
		Sleep(5000);
		return NULL;
	}
	else
	{
		SetConsoleTextAttribute(hConsole, 2);
		std::cout << "SUCCESS (" << globals::csgo_process_id << ")" << std::endl;
	}

	SetConsoleTextAttribute(hConsole, 7);
	std::cout << "\nTrying to locate base modules... " << std::endl;
	globals::client_module_address = get_module_base_address_x86(globals::csgo_process_id, "client.dll");
	globals::game_base_address = get_module_base_address(globals::csgo_process_id, "csgo.exe");

	std::cout << "csgo.exe: ";

	if (!globals::game_base_address) {
		SetConsoleTextAttribute(hConsole, 4);
		std::cout << "NOT FOUND" << std::endl;
		Sleep(5000);
		return NULL;
	}
	else
	{
		SetConsoleTextAttribute(hConsole, 2);
		std::cout << "SUCCESS: (0x" << std::hex << globals::game_base_address << std::dec << ")" << std::endl;
	}

	SetConsoleTextAttribute(hConsole, 7);
	std::cout << "client.dll: ";
	
	if (!globals::client_module_address) {
		SetConsoleTextAttribute(hConsole, 4);
		std::cout << "NOT FOUND" << std::endl;
		Sleep(5000);
		return NULL;
	}
	else
	{
		SetConsoleTextAttribute(hConsole, 2);
		std::cout << "SUCCESS: (0x" << std::hex << globals::client_module_address << std::dec << ")" << std::endl;
	}

	SetConsoleTextAttribute(hConsole, 2);
	std::cout << "\nStarting in 3s...\n\n" << std::endl;
	SetConsoleTextAttribute(hConsole, 7);
	Sleep(3000);

	bool F2 = false;

	while (true)
	{
		// ==== Dynamic ESP ====
		if (GetAsyncKeyState(VK_F2) < 0 && F2 == false)
		{
			globals::cEspBoxes = !globals::cEspBoxes;
			//const char* cEspState = globals::cEspBoxes ? "[ON]" : "[OFF]"

			std::cout << "Player/Zombies ESP: ";
			if (globals::cEspBoxes) {
				SetConsoleTextAttribute(hConsole, 2);
				std::cout << "[ON]" << std::endl;
			}
			else {
				SetConsoleTextAttribute(hConsole, 4);
				std::cout << "[OFF]" << std::endl;
			}
			SetConsoleTextAttribute(hConsole, 7);

			F2 = true;
		}
		if (GetAsyncKeyState(VK_F2) == 0 && F2 == true) F2 = false;


		if (globals::cEspBoxes)
		{
			DWORD LocalPlayerPointer = Read<DWORD>(globals::client_module_address + offsets::dwLocalPlayer);
			view_matrix_t ViewMatrix = Read<view_matrix_t>(globals::client_module_address + offsets::dwViewMatrix);

			// check if player is valid
			if (LocalPlayerPointer != 0)
			{
				int LocalPlayerHealth = Read<int>(LocalPlayerPointer + offsets::m_iHealth);
				int LocalPlayerTeam = Read<int>(LocalPlayerPointer + offsets::m_iTeamNum);

				for (int i = 0; i < 32; i++)
				{
					DWORD EntityPointer = Read<DWORD>(globals::client_module_address + offsets::dwEntityList + i * 0x10);
					int EntityDormant = Read<int>(EntityPointer + offsets::m_bDormant);
					int EntityHealth = Read<int>(EntityPointer + offsets::m_iHealth);
					int EntityTeam = Read<int>(EntityPointer + offsets::m_iTeamNum);

					if (EntityHealth == 0 || EntityDormant != 0 || EntityTeam == LocalPlayerTeam || EntityPointer == LocalPlayerPointer)
						continue;

					CVector EntityPos = Read<CVector>(EntityPointer + offsets::m_vecOrigin);
					CVector EntityHead;
					EntityHead.fX = EntityPos.fX;
					EntityHead.fY = EntityPos.fY;
					EntityHead.fZ = EntityPos.fZ + 65.f;
					CVector ScreenPos;
					CVector ScreenHead;



					if (W2SCSGO(ViewMatrix, EntityPos, ScreenPos) && W2SCSGO(ViewMatrix, EntityHead, ScreenHead))
					{
						float height = ScreenPos.fY - ScreenHead.fY;
						float width = height / 2.4f;
						draw::box(ScreenHead.fX - (width / 2), ScreenHead.fY, width, height, 2, 255, 0, 0);
					}
				}
			}
		}
	}
}