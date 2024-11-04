#include <Windows.h>
#include <functional>
#include <GL/gl.h>
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl2.h"
#include <font.h>

#define _CAST(t, v) reinterpret_cast<t>(v)
#define _VOID_1(v) std::function<void(v)>
#define _VOID_2(v) _VOID_1(_VOID_1(v))

typedef BOOL(__stdcall *wglSwapBuffers_t)(
		HDC hDc);

typedef LRESULT(CALLBACK *WNDPROC)(
		IN HWND hwnd,
		IN UINT uMsg,
		IN WPARAM wParam,
		IN LPARAM lParam);

extern LRESULT ImGui_ImplWin32_WndProcHandler(
		HWND hWnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam);

extern void RenderMain();

std::function<void(int)> keyPressHandler = [](int _) {};

namespace ImGuiHook
{
	// Original functions variable
	static WNDPROC o_WndProc;
	static wglSwapBuffers_t o_wglSwapBuffers;

	// Global variable
	static HGLRC g_WglContext;
	static bool initImGui = false;
	static _VOID_1() RenderMain;

	// WndProc callback ImGui handler
	LRESULT CALLBACK h_WndProc(
			const HWND hWnd,
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam)
	{
		/*if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;

		return CallWindowProc(o_WndProc, hWnd, uMsg, wParam, lParam);*/
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

		if (uMsg == WM_KEYDOWN && !ImGui::GetIO().WantCaptureKeyboard)
		{
			keyPressHandler(wParam);
		}

		bool blockInput = false;
		if (ImGui::GetIO().WantCaptureMouse)
		{
			switch (uMsg)
			{
			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_MBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MOUSEACTIVATE:
			case WM_MOUSEHOVER:
			case WM_MOUSEHWHEEL:
			case WM_MOUSELEAVE:
			case WM_MOUSEMOVE:
			case WM_MOUSEWHEEL:
			case WM_NCLBUTTONDBLCLK:
			case WM_NCLBUTTONDOWN:
			case WM_NCLBUTTONUP:
			case WM_NCMBUTTONDBLCLK:
			case WM_NCMBUTTONDOWN:
			case WM_NCMBUTTONUP:
			case WM_NCMOUSEHOVER:
			case WM_NCMOUSELEAVE:
			case WM_NCMOUSEMOVE:
			case WM_NCRBUTTONDBLCLK:
			case WM_NCRBUTTONDOWN:
			case WM_NCRBUTTONUP:
			case WM_NCXBUTTONDBLCLK:
			case WM_NCXBUTTONDOWN:
			case WM_NCXBUTTONUP:
			case WM_RBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_XBUTTONDBLCLK:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
				blockInput = true;
			}
		}

		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			switch (uMsg)
			{
			case WM_HOTKEY:
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_KILLFOCUS:
			case WM_SETFOCUS:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
				blockInput = true;
			}
		}

		if (blockInput)
			return true;

		return CallWindowProc(o_WndProc, hWnd, uMsg, wParam, lParam);
	}

	// Helper function
	void ExitStatus(bool *status, bool value)
	{
		if (status)
			*status = value;
	}

	HWND hWnd = nullptr;

	// Initialisation for ImGui
	void InitOpenGL2(
			IN HDC hDc,
			OUT bool *init,
			OUT bool *status)
	{
		if (WindowFromDC(hDc) == hWnd && *init)
			return;
		auto tStatus = true;

		hWnd = WindowFromDC(hDc);
		auto wLPTR = SetWindowLongPtr(hWnd, GWLP_WNDPROC, _CAST(LONG_PTR, h_WndProc));

		if (*init)
		{
			ImGui_ImplWin32_Init(hWnd);
			ImGui_ImplOpenGL2_Init();
			return;
		}

		if (!wLPTR)
			return ExitStatus(status, false);

		o_WndProc = _CAST(WNDPROC, wLPTR);
		g_WglContext = wglCreateContext(hDc);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		tStatus &= ImGui_ImplWin32_Init(hWnd);
		tStatus &= ImGui_ImplOpenGL2_Init();

		ImFontConfig font;
		font.FontDataOwnedByAtlas = false;
		ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void *)fontData, sizeof(fontData), 18.0f, &font);

		*init = true;
		return ExitStatus(status, tStatus);
	}

	// Generic ImGui renderer for Win32 backend
	void RenderWin32(
			IN std::function<void()> render)
	{
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		render();

		ImGui::EndFrame();
		ImGui::Render();
	}

	// Generic ImGui renderer for OpenGL2 backend
	void RenderOpenGL2(
			IN HGLRC WglContext,
			IN HDC hDc,
			IN _VOID_2() render,
			IN _VOID_1() render_inner,
			OUT bool *status)
	{
		auto tStatus = true;

		auto o_WglContext = wglGetCurrentContext();
		tStatus &= wglMakeCurrent(hDc, WglContext);

		ImGui_ImplOpenGL2_NewFrame();
		render(render_inner);
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		tStatus &= wglMakeCurrent(hDc, o_WglContext);

		return ExitStatus(status, tStatus);
	}

	// Hooked wglSwapBuffers function
	BOOL __stdcall h_wglSwapBuffers(
			IN HDC hDc)
	{
		InitOpenGL2(hDc, &initImGui, nullptr);
		RenderOpenGL2(g_WglContext, hDc, RenderWin32, RenderMain, nullptr);

		return o_wglSwapBuffers(hDc);
	}

	// Function to get the pointer of wglSwapBuffers
	wglSwapBuffers_t *get_wglSwapBuffers()
	{
		auto hMod = GetModuleHandleA("OPENGL32.dll");
		if (!hMod)
			return nullptr;

		return (wglSwapBuffers_t *)GetProcAddress(hMod, "wglSwapBuffers");
	}

	// Initialise hook
	bool InitHook()
	{
		if (kiero::init(kiero::RenderType::Auto) == kiero::Status::Success)
			return kiero::bind(get_wglSwapBuffers(), (void **)&o_wglSwapBuffers, h_wglSwapBuffers) == kiero::Status::Success;

		return false;
	}

	// Main load function
	bool Load(
			IN _VOID_1() render)
	{
		RenderMain = render;
		return InitHook();
	}

	// Main unload function
	void Unload()
	{
		kiero::shutdown();
	}
}
