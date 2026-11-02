module;

#include <Windows.h>

export module Engine.Input:InputHandler;

export namespace engine
{
	class InputHandler
	{
	public:
		void Initialize(HWND hwnd);
		void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

		void GetMouseMoveAndReset(LONG& x, LONG& y);
		void ResetMouseMove();

		void CaptureMouse();
		void ReleaseMouse();

		[[nodiscard]] bool IsKeyPressed(char key) const;
		[[nodiscard]] bool IsKeyPressed(int button) const;

	private:
		HWND m_hwnd;
		LONG m_mouseMoveX = 0;
		LONG m_mouseMoveY = 0;
		bool m_wasRightMousePressedLastFrame = false;
		bool m_isMouseCaptured = false;
	};
	
}