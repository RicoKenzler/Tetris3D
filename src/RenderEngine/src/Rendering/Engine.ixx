module;

#include <Windows.h>

export module RenderEngine.Rendering:Engine;

import :Renderer;
import :Camera;
import RenderEngine.Input;

export namespace RenderEngine
{
	class Engine
	{
	public:
		Engine() = default;

		void Start(HINSTANCE instance, int showCommand);
		int Run();

	private:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		void InitWindow(HINSTANCE instance);
		void ProcessInput(float deltaTime);
		void CaptureMouse();
		void ReleaseMouse();
		
		bool m_wasRightMousePressedLastFrame = false;
		bool m_isMouseCaptured = false;
		InputHandler m_inputHandler;
		Renderer m_renderer;
		Camera m_camera;
		HWND m_hwnd = nullptr;
	};
}