module;

#include <Windows.h>

#include <stdexcept>

#include <string>

module RenderEngine.Rendering;

import :Engine;

namespace RenderEngine
{
	constexpr UINT WindowWidth = 1280;
	constexpr UINT WindowHeight = 720;


	void Engine::Start(HINSTANCE instance, int showCommand)
	{
		InitWindow(instance);

		m_renderer.Initialize(m_hwnd, WindowWidth, WindowHeight);

		ShowWindow(m_hwnd, showCommand);
	}
	
	int Engine::Run()
	{
		MSG msg{};
		bool running = true;

		while (running)
		{
			while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					running = false;
					break;
				}

				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}

			if (!running)
				break;

			m_renderer.Render();
		}

		m_renderer.ShutDown();

		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK Engine::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_DESTROY)
		{
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProcW(hwnd, message, wParam, lParam);
	}

	void Engine::InitWindow(HINSTANCE instance)
	{
		WNDCLASSW windowClass{};
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = instance;
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.lpszClassName = L"MiniRenderWindow";

		if (!RegisterClassW(&windowClass))
			throw std::runtime_error("RegisterClass failed");

		// TODO: add resize functionality later
		const DWORD windowStyle =
			WS_OVERLAPPED |
			WS_CAPTION |
			WS_SYSMENU |
			WS_MINIMIZEBOX;

		RECT windowRect{
			0, 0,
			static_cast<LONG>(WindowWidth),
			static_cast<LONG>(WindowHeight)
		};

		if (!AdjustWindowRect(&windowRect, windowStyle, FALSE))
			throw std::runtime_error("AdjustWindowRect failed");

		m_hwnd = CreateWindowExW(
			0,
			windowClass.lpszClassName,
			L"Mini Render Engine",
			windowStyle,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,
			nullptr,
			instance,
			nullptr
		);

		if (!m_hwnd)
			throw std::runtime_error("CreateWindow failed");
	}
}