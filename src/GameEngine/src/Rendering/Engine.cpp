module;

#include <Windows.h>
#include <stdexcept>

module Engine.Rendering;

import :Engine;
import :Time;
import Engine.Input;

namespace engine
{
	constexpr UINT WindowWidth = 1280;
	constexpr UINT WindowHeight = 720;

	void Engine::Initialize(HINSTANCE instance, int showCommand, core::IGame* game)
	{
		InitWindow(instance);

		HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		CheckHR(result, "CoInitializeEx failed");

		m_renderer.Initialize(m_hwnd, WindowWidth, WindowHeight);

		ShowWindow(m_hwnd, showCommand);

		m_game = game;

		m_inputHandler.Initialize(m_hwnd);
	}
	
	int Engine::Run()
	{
		m_time.Init();

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

			m_time.Update();
			m_game->Update(m_time.GetDeltaTime());
			m_renderer.Render(m_time.GetElapsedSeconds(), m_camera);
		}

		return static_cast<int>(msg.wParam);
	}

	bool Engine::HasFocus() const
	{
		const bool hasFocus = GetForegroundWindow() == m_hwnd;
		return hasFocus;
	}

	LRESULT CALLBACK Engine::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_INPUT)
		{
			const LONG_PTR enginePtr = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
			if (enginePtr != 0)
			{
				Engine* const engine = reinterpret_cast<Engine*>(enginePtr);
				engine->m_inputHandler.ProcessMessage(message, wParam, lParam);
			}
		}
		else if (message == WM_NCDESTROY)
		{
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
		}
		else if (message == WM_DESTROY)
		{
			const LONG_PTR enginePtr = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
			if (enginePtr != 0)
			{
				Engine* const engine = reinterpret_cast<Engine*>(enginePtr);
				engine->m_inputHandler.ReleaseMouse();
			}

			CoUninitialize();
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProcW(hwnd, message, wParam, lParam);
	}

	void Engine::CheckHR(HRESULT result, const char* message)
	{
		if (FAILED(result))
			throw std::runtime_error(message);
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

		SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	}
}