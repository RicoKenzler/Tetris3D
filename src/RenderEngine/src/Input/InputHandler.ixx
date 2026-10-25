module;

#include <Windows.h>

export module RenderEngine.Input:InputHandler;

export namespace RenderEngine
{
	class InputHandler
	{
	public:
		void Initialize(HWND hwnd);
		void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

		void GetMouseMoveAndReset(LONG& x, LONG& y);
		void ResetMouseMove();

	private:
		LONG m_mouseMoveX = 0;
		LONG m_mouseMoveY = 0;
	};
	
}