#include <Windows.h>
#include "include/imgui_hook.h"
#include "include/imgui/imgui.h"
#include "include/kiero/minhook/include/MinHook.h"
#include "include/style.h"
#include "include/player_t.h"
#include "include/aactor.h"
#include "include/FLevelLocals.h"
#include "libmem/libmem.hpp"
#include <minhook/include/MinHook.h>

#define fnptr(retType, name, ...) retType (*name)(__VA_ARGS__)

#pragma region vars
bool initalized = false;
bool menu_open = true;
bool debug_mode = false;

float screenWidth = (float)GetSystemMetrics(SM_CXSCREEN);
float screenHeight = (float)GetSystemMetrics(SM_CYSCREEN);
float menu_depth = 4;
float menu_size = (screenWidth + screenHeight) / menu_depth;
float scale_factor = 0.0f;
bool animating = false;
float animation_duration = 0.3f;
float animation_start_time = 0.0f;
int page = 0;

char popup_message[256];
int popup_frame = 0;

int target_hud_frame = 0;
AActor *target_hud_actor = nullptr;

int hooked_functions = 0;
#pragma endregion

bool invincibility = false;
bool one_hit_kill = false;
bool cant_hit_yourself = false;
bool infinite_melee_range = false;

namespace GZDoom
{
	uintptr_t base;
	uintptr_t doDamage;

	FLevelLocals *level = 0;
	AActor *local_player_actor = 0;

	float *fov = 0;

	typedef fnptr(int, AActorGetMaxHealth_t, AActor *, bool);
	AActorGetMaxHealth_t AActorGetMaxHealth;

	fnptr(void, DoDamageMobj, AActor *dmgtarget, AActor *inflictor, AActor *source, int amount, short fname, int flags, long angle);
	void DoDamageMobj_Hook(AActor *dmgtarget, AActor *inflictor, AActor *source, int amount, short fname, int flags, long angle)
	{
		if (source == local_player_actor || true)
		{
			if (one_hit_kill)
			{
				amount = *(int *)((uintptr_t)dmgtarget + 0x2A0);
			}
			if (cant_hit_yourself)
			{
				if (dmgtarget == local_player_actor && source == local_player_actor)
				{
					amount = 0;
				}
			}

			snprintf(popup_message, sizeof(popup_message), "Dealt %d damage", amount);
			if (debug_mode)
			{
				snprintf(popup_message, sizeof(popup_message), "%p Dealt %d damage to %p", source, amount, dmgtarget);
			}
			popup_frame = 500;
		}
		if (invincibility)
		{
			if (dmgtarget == local_player_actor)
			{
				amount = 0;
			}
		}

		if (target_hud_actor != dmgtarget && source == GZDoom::local_player_actor)
		{
			target_hud_actor = dmgtarget;
			target_hud_frame = 1000;
		}
		DoDamageMobj(dmgtarget, inflictor, source, amount, fname, flags, angle);
	}

	fnptr(__int64, PatchAmmo, unsigned int ammoNum, int flags);
	__int64 PatchAmmo_Hook(unsigned int ammoNum, int flags)
	{
		if (ammoNum < 0)
		{
			ammoNum = 0;
		}
		ammoNum = 0;

		return PatchAmmo(ammoNum, flags);
	}
};

float EaseInOut(float t)
{
	if (t < 0.5f)
		return 2.0f * t * t; // Ease in
	else
		return -1.0f + (4.0f - 2.0f * t) * t; // Ease out
}

#define CreateAndEnableHook(from, to, trampoline)                               \
	if (MH_CreateHook((LPVOID)(from), (LPVOID)to, (LPVOID *)trampoline) == MH_OK) \
		if (MH_EnableHook((LPVOID)(from)) == MH_OK)                                 \
			hooked_functions++;
void RenderMain()
{
	float current_time = (float)GetTickCount() / 1000.0f;
	if (!initalized)
	{
		GZDoom::base = (uintptr_t)libmem::FindModule("gzdoom.exe")->base;

		CreateAndEnableHook(GZDoom::base + 0x259D40, &GZDoom::DoDamageMobj_Hook, &GZDoom::DoDamageMobj);
		CreateAndEnableHook(GZDoom::base + 0x2D22C0, &GZDoom::PatchAmmo_Hook, &GZDoom::PatchAmmo);

		GZDoom::level = (FLevelLocals *)(GZDoom::base + 0x1244F10);
		GZDoom::AActorGetMaxHealth = (GZDoom::AActorGetMaxHealth_t)(GZDoom::base + 0x27F510);

		SetupImGuiStyle();
		initalized = true;

		animation_start_time = current_time;
		animating = true;
		menu_open = true;
	}
	GZDoom::local_player_actor = (AActor *)GZDoom::level->Players[0];

	{
		// Update scale factor based on menu state and animation
		if (animating)
		{
			float elapsed_time = current_time - animation_start_time;
			float t = elapsed_time / animation_duration;

			if (t >= 1.0f) // Animation complete
			{
				t = 1.0f;
				animating = false; // Stop animating
			}

			// Apply easing function
			float eased_t = EaseInOut(t);
			scale_factor = menu_open ? eased_t : 1.0f - eased_t; // Scale up if opening, scale down if closing
		}
		else
		{
			scale_factor = menu_open ? 1.0f : 0.0f; // Set scale factor directly if not animating
		}

		// Set the window size based on the scale factor
		float scaled_height = (screenHeight - menu_size) * scale_factor;
		ImGui::SetNextWindowSize({screenWidth - menu_size, scaled_height});
		ImGui::SetNextWindowPos({menu_size / 2, (screenHeight - scaled_height) / 2}); // Center vertically
	}

	float button_width = (screenWidth - menu_size) / 3 - 10;

	if (scale_factor > 0.0f)
	{
		ImGui::Begin("aceinetx.gzdoom RECODE", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

		if (ImGui::Button("Hacks", {button_width, 50}))
		{
			page = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Misc", {button_width, 50}))
		{
			page = 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Debug info", {button_width, 50}))
		{
			page = 2;
		}
		ImGui::Separator();

		switch (page)
		{
		case 0:
			ImGui::Checkbox("Invincibility", &invincibility);
			ImGui::Checkbox("One hit kill", &one_hit_kill);
			ImGui::Checkbox("Can't hit yourself", &cant_hit_yourself);

			if (GZDoom::local_player_actor != 0)
			{
				ImGui::InputDouble("Gravity", &GZDoom::local_player_actor->Gravity);
				ImGui::InputDouble("Speed", &GZDoom::local_player_actor->Speed);

				ImGui::Checkbox("Infinite melee range", &infinite_melee_range);
				if (infinite_melee_range)
				{
					GZDoom::local_player_actor->MeleeRange = 99999999;
				}
				else
				{
					GZDoom::local_player_actor->MeleeRange = 44;
				}
			}
			else
			{
				ImGui::Text("!! You must be in a level to use other hacks");
			}

			break;
		case 1:
			ImGui::Checkbox("Debug mode", &debug_mode);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Enables debug mode. This shows addresses / variables");
				ImGui::EndTooltip();
			}
			if (ImGui::Button("Unload DLL"))
			{
				ImGui::GetIO().WantCaptureMouse = false;
				ImGuiHook::Unload();
			}
			break;
		case 2:
			ImGui::Text("Hooked functions: %d", hooked_functions);

			if (debug_mode)
			{
				ImGui::Text("Local player (actor): %p", GZDoom::local_player_actor);
				ImGui::Text("Level: %p", GZDoom::level);

				if (GZDoom::local_player_actor != 0)
				{
					ImGui::Text("Health: %d", GZDoom::local_player_actor->Health);
				}
			}
			break;
		}

		ImGui::End();
	}
	else
	{
		ImGui::GetIO().WantCaptureMouse = false;
	}

	if (popup_frame > 0)
	{
		ImGui::SetNextWindowSize({500, 30});
		ImGui::SetNextWindowPos({50, screenHeight - 50});
		ImGui::Begin("popup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::Text("%s", popup_message);
		ImGui::End();
		popup_frame--;
	}

	if (target_hud_frame > 0 && target_hud_actor != nullptr)
	{
		ImGui::SetNextWindowSize({120, 30});
		ImGui::SetNextWindowPos({0, screenHeight / 2});
		ImGui::Begin("targethud", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::ProgressBar((float)target_hud_actor->Health / GZDoom::AActorGetMaxHealth(target_hud_actor, true), {100, 15});
		ImGui::End();
		target_hud_frame--;

		if (target_hud_actor->Health <= 0)
		{
			target_hud_frame = 0;
		}

		if (target_hud_frame <= 0)
		{
			target_hud_actor = nullptr;
		}
	}

	if (GetAsyncKeyState('K') & 0x1)
	{
		menu_open = !menu_open;
		animating = true;
		animation_start_time = current_time;
	}
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		menu_open = false;
		DisableThreadLibraryCalls(hMod);
		ZeroMemory(popup_message, sizeof(popup_message));
		ImGuiHook::Load(RenderMain);
		break;
	case DLL_PROCESS_DETACH:
		ImGuiHook::Unload();
		break;
	}
	return TRUE;
}
