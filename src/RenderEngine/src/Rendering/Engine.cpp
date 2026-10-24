module;

#include <DirectXMath.h>
#include <array>
#include <Windows.h>
#include <chrono>
#include <stdexcept>
#include <string>
#include <algorithm>

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
		using Clock = std::chrono::steady_clock;

		const auto startTime = Clock::now();

		MSG msg{};
		bool running = true;

		float lastFrameTime = 0;

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

			// Calculate the elapsed time since the engine started.
			const auto now = Clock::now();

			const std::chrono::duration<float> elapsed = now - startTime;

			const float elapsedSeconds = elapsed.count();

			const float deltaTime = elapsedSeconds - lastFrameTime;
			ProcessInput(deltaTime);
			lastFrameTime = elapsedSeconds;

			// Render the scene at the current time.
			m_renderer.Render(elapsedSeconds, m_camera);
		}

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

	void Engine::ProcessInput(const float deltaTime)
	{
		const bool hasFocus = GetForegroundWindow() == m_hwnd;
		if (!hasFocus)
			return;
		
		const bool wPressed = (GetAsyncKeyState('W') & 0x8000) != 0;
		const bool aPressed = (GetAsyncKeyState('A') & 0x8000) != 0;
		const bool sPressed = (GetAsyncKeyState('S') & 0x8000) != 0;
		const bool dPressed = (GetAsyncKeyState('D') & 0x8000) != 0;
		const bool leftPressed = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
		const bool rightPressed = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
		const bool upPressed = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
		const bool downPressed = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;

		constexpr float rotationSpeed = DirectX::XMConvertToRadians(90.0f);
		const float cameraSpeed = 4.0f; // Units per second
		DirectX::XMFLOAT3 position = m_camera.GetPosition();
		DirectX::XMFLOAT3 up = m_camera.GetUp();
		
		DirectX::XMFLOAT3 step = { 0.0f, 0.0f, 0.0f};
		DirectX::XMVECTOR stepVec = DirectX::XMLoadFloat3(&step);
		
		float yaw = m_camera.GetYaw();
		if (leftPressed)
		{
			yaw -= rotationSpeed * deltaTime;
		}
		if (rightPressed)
		{
			yaw += rotationSpeed * deltaTime;
		}
		m_camera.SetYaw(yaw);

		float pitch = m_camera.GetPitch();
		if (upPressed)
		{
			pitch += rotationSpeed * deltaTime;
		}
		if (downPressed)
		{
			pitch -= rotationSpeed * deltaTime;
		}

		const float maxPitch = DirectX::XMConvertToRadians(89.0f);
		pitch = std::clamp(pitch, -maxPitch, maxPitch);
		m_camera.SetPitch(pitch);

		DirectX::XMVECTOR directionVec = m_camera.CalculateDirection();

		//only walk on ground plane
		DirectX::XMFLOAT3 direction = { 0.0f, 0.0f, 0.0f };
		DirectX::XMStoreFloat3(&direction, directionVec);
		direction.y = 0.0f;
		directionVec = DirectX::XMLoadFloat3(&direction);
		directionVec = DirectX::XMVector3Normalize(directionVec);

		DirectX::XMVECTOR positionVec = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR left = DirectX::XMVector3Cross(directionVec, DirectX::XMLoadFloat3(&up));

		if (wPressed)
		{
			stepVec = DirectX::XMVectorAdd(stepVec, directionVec);
		}
		if (sPressed)
		{
			stepVec = DirectX::XMVectorSubtract(stepVec, directionVec);
		}
		if (aPressed)
		{
			stepVec = DirectX::XMVectorAdd(stepVec, left);
		}
		if (dPressed)
		{
			stepVec = DirectX::XMVectorSubtract(stepVec, left);
		}


		stepVec = DirectX::XMVector3Normalize(stepVec);
		stepVec = DirectX::XMVectorScale(stepVec, cameraSpeed * deltaTime);
		positionVec = DirectX::XMVectorAdd(positionVec, stepVec);
		DirectX::XMStoreFloat3(&position, positionVec);

		m_camera.SetPosition(position);
	}
}