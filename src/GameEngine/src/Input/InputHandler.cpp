module;

#include <Windows.h>
#include <stdexcept>
#include <string>
#include <vector>

module Engine.Input;

import :InputHandler;

namespace engine
{
	void InputHandler::Initialize(HWND hwnd)
	{
		m_hwnd = hwnd;

		RAWINPUTDEVICE mouse{};
		mouse.usUsagePage = 0x01;	// HD Generic Desktop Controls
		mouse.usUsage = 0x02;		// mouse
		mouse.dwFlags = 0;			// input on focus
		mouse.hwndTarget = hwnd;

		if (!RegisterRawInputDevices(&mouse, 1, sizeof(RAWINPUTDEVICE)))
			throw std::runtime_error("RegisterRawInputDevices failed. Error: " + std::to_string(GetLastError()));
	}
	
	void InputHandler::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		UINT dataSize = 0;

		const UINT result = GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			nullptr,
			&dataSize,
			sizeof(RAWINPUTHEADER)
		);

		if (result != 0)
			return;

		std::vector<DWORD> buffer((dataSize + sizeof(DWORD) - 1) / sizeof(DWORD));

		const UINT readResult = GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			buffer.data(),
			&dataSize,
			sizeof(RAWINPUTHEADER)
		);

		if (readResult == static_cast<UINT>(-1))
			return;

		const RAWINPUT* const rawInput = reinterpret_cast<RAWINPUT*>(buffer.data());
		if (!rawInput)
			return;

		if (rawInput->header.dwType == RIM_TYPEMOUSE)
		{
			const RAWMOUSE& rawMouse = rawInput->data.mouse;
			const USHORT ignoredFlags = MOUSE_MOVE_ABSOLUTE | MOUSE_ATTRIBUTES_CHANGED;
			if ((rawMouse.usFlags & ignoredFlags) == 0)
			{
				m_mouseMoveX += rawMouse.lLastX;
				m_mouseMoveY += rawMouse.lLastY;
			}
		}
	}

	void InputHandler::GetMouseMoveAndReset(LONG& x, LONG& y)
	{
		x = m_mouseMoveX;
		y = m_mouseMoveY;
		m_mouseMoveX = 0;
		m_mouseMoveY = 0;
	}

	void InputHandler::ResetMouseMove()
	{
		m_mouseMoveX = 0;
		m_mouseMoveY = 0;
	}

	void InputHandler::CaptureMouse()
	{
		if (m_isMouseCaptured)
			return;

		RECT rect{};
		if (!GetClientRect(m_hwnd, &rect))
			return;

		POINT br{};
		br.x = rect.right;
		br.y = rect.bottom;
		if (!ClientToScreen(m_hwnd, &br))
			return;

		POINT tl{};
		tl.x = rect.left;
		tl.y = rect.top;
		if (!ClientToScreen(m_hwnd, &tl))
			return;

		rect.left = tl.x;
		rect.top = tl.y;
		rect.right = br.x;
		rect.bottom = br.y;
		if (!ClipCursor(&rect))
			return;

		m_isMouseCaptured = true;
		ShowCursor(false);
	}

	void InputHandler::ReleaseMouse()
	{
		if (!m_isMouseCaptured)
			return;

		ShowCursor(true);
		ClipCursor(nullptr);
		m_isMouseCaptured = false;
	}

	bool InputHandler::IsKeyPressed(const char key) const
	{
		const bool isPressed = (GetAsyncKeyState(key) & 0x8000) != 0;
		return isPressed;
	}

	bool InputHandler::IsKeyPressed(const int button) const
	{
		const bool isPressed = (GetAsyncKeyState(button) & 0x8000) != 0;
		return isPressed;
	}
}