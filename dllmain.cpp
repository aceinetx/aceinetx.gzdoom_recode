#include <Windows.h>
#include <imgui_hook.h>
#include <imgui.h>
#include <MinHook.h>
#include <style.h>
#include <player_t.h>
#include <aactor.h>
#include <FLevelLocals.h>
#include <libmem/libmem.hpp>
#include <minhook/include/MinHook.h>
#include <gzdoom.h>
#include <interface.h>
#include <globals.h>

float EaseInOut(float t)
{
	if (t < 0.5f)
		return 2.0f * t * t; // Ease in
	else
		return -1.0f + (4.0f - 2.0f * t) * t; // Ease out
}

void RenderProgressBar(float targetProgress, float deltaTime, const ImVec2 &size_arg = ImVec2(-(1.1754944E-38F), 0))
{
	static float currentProgress = 0.0f; // Current progress value
	static float animationSpeed = 2.0f;	 // Speed of the animation

	// Update current progress towards target progress
	if (std::abs(currentProgress - targetProgress) > 0.01f)
	{
		currentProgress += (targetProgress - currentProgress) * animationSpeed * deltaTime;
	}
	else
	{
		currentProgress = targetProgress; // Snap to target if close enough
	}

	// Render the progress bar
	ImGui::ProgressBar(currentProgress, ImVec2(0.0f, 0.0f));
}

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
		ImGui::Begin("aceinetx.gzdoom RECODE (for gzdoom g4.13.1)", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

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
			ImGui::Checkbox("Target HUD", &target_hud);

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
		ImVec4 backgroundColor = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, backgroundColor);

		ImGui::SetNextWindowSize({500, 50});
		ImGui::SetNextWindowPos({screenWidth / 2 - 500 / 2, screenHeight - 200});
		ImGui::Begin("popup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::Text("%s", popup_message);
		ImGui::End();
		popup_frame--;

		ImGui::PopStyleColor();
	}

	if (target_hud_frame > 0 && target_hud_actor != nullptr)
	{
		if (target_hud)
		{
			ImGui::SetNextWindowSize({120, 50});
			ImGui::SetNextWindowPos({screenWidth / 2 - 120 / 2, screenHeight / 2 + 20});
			ImGui::Begin("targethud", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
			RenderProgressBar((float)target_hud_actor->Health / GZDoom::AActorGetMaxHealth(target_hud_actor, true), 0.016f, {100, 15});
			ImGui::End();
		}
		target_hud_frame--;

		if (target_hud_actor->Health <= 0 && target_hud_wait_until_close == false)
		{
			target_hud_frame = 200;
			target_hud_wait_until_close = true;
		}

		if (target_hud_frame <= 0)
		{
			target_hud_actor = nullptr;
			target_hud_wait_until_close = false;
		}
	}
	RenderInterface();

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
