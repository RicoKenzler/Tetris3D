module;

#include <Windows.h>

export module Engine.Rendering:Engine;

import Core;
import :Renderer;
import :Camera;
import :Time;
import Engine.Input;

export namespace engine
{
	class Engine
	{
	public:
		Engine() = default;

		void Initialize(HINSTANCE instance, int showCommand, core::IGame* game);
		int Run();

		[[nodiscard]] bool HasFocus() const;

		const InputHandler& GetInputHandler() const { return m_inputHandler; }
		InputHandler& GetInputHandler() { return m_inputHandler; }

		const Camera& GetCamera() const { return m_camera; }
		Camera& GetCamera() { return m_camera; }

	private:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static void CheckHR(HRESULT result, const char* message);

		void InitWindow(HINSTANCE instance);
		
		Time m_time;
		InputHandler m_inputHandler;
		Renderer m_renderer;
		Camera m_camera;
		core::IGame* m_game = nullptr;
		HWND m_hwnd = nullptr;
	};
}