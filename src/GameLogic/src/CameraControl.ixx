export module GameLogic:CameraControl;

export namespace game
{
	class CameraControl
	{
	public:
		CameraControl() = default;
		void UpdateCamera(float deltaTime);
	private:
		bool m_wasRightMousePressedLastFrame = false;
	};

}