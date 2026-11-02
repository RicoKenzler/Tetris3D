module;

#include <Windows.h>

export module Engine.Rendering:Engine;

export import :Renderer;
export import :Camera;
export import :Time;
export import Engine.Input;
import Core;

export namespace engine
{
	class Engine
	{
	public:
		Engine() = default;

		void Initialize(HINSTANCE instance, int showCommand, core::IGame* game);
		int Run();
		void Shutdown();

		[[nodiscard]] bool HasFocus() const;

		const InputHandler& GetInputHandler() const { return m_inputHandler; }
		InputHandler& GetInputHandler() { return m_inputHandler; }

		const Camera& GetCamera() const { return m_camera; }
		Camera& GetCamera() { return m_camera; }

		const Renderer& GetRenderer() const { return m_renderer; }
		Renderer& GetRenderer() { return m_renderer; }

		const Time& GetTime() const { return m_time; }

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