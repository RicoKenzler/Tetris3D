module;

#include <Windows.h>

export module RenderEngine.Rendering:Engine;

import :Renderer;

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
		
		Renderer m_renderer;
		HWND m_hwnd = nullptr;
	};
}