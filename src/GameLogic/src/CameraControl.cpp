module;

#include <Windows.h>
#include <DirectXMath.h>
#include <algorithm>

module GameLogic;

import Engine;
import :CameraControl;

namespace game
{
	void CameraControl::UpdateCamera(const float deltaTime)
	{
		engine::InputHandler& inputHandler = Game::GetEngine()->GetInputHandler();

		const bool hasFocus = Game::GetEngine()->HasFocus();

		if (!hasFocus)
		{
			inputHandler.ReleaseMouse();
			inputHandler.ResetMouseMove();
			m_wasRightMousePressedLastFrame = false;
			return;
		}

		engine::Camera& camera = Game::GetEngine()->GetCamera();

		const bool wPressed = inputHandler.IsKeyPressed('W');
		const bool aPressed = inputHandler.IsKeyPressed('A');
		const bool sPressed = inputHandler.IsKeyPressed('S');
		const bool dPressed = inputHandler.IsKeyPressed('D');
		const bool rightMousePressed = inputHandler.IsKeyPressed(VK_RBUTTON);

		constexpr float rotationSpeed = DirectX::XMConvertToRadians(25.0f); // degrees per 100 pixels
		const float cameraSpeed = 4.0f; // Units per second
		DirectX::XMFLOAT3 position = camera.GetPosition();
		DirectX::XMFLOAT3 up = camera.GetUp();

		DirectX::XMFLOAT3 step = { 0.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR stepVec = DirectX::XMLoadFloat3(&step);

		float yaw = camera.GetYaw();
		float pitch = camera.GetPitch();

		LONG mouseMoveX = 0;
		LONG mouseMoveY = 0;
		inputHandler.GetMouseMoveAndReset(mouseMoveX, mouseMoveY);

		if (rightMousePressed)
		{
			inputHandler.CaptureMouse();
		}
		else if (!rightMousePressed)
		{
			inputHandler.ReleaseMouse();
		}

		if (rightMousePressed && m_wasRightMousePressedLastFrame)
		{
			yaw += rotationSpeed * mouseMoveX / 100.0f;
			pitch += rotationSpeed * -mouseMoveY / 100.0f;

			const float maxPitch = DirectX::XMConvertToRadians(89.0f);
			pitch = std::clamp(pitch, -maxPitch, maxPitch);

			camera.SetPitch(pitch);
			camera.SetYaw(yaw);
		}
		m_wasRightMousePressedLastFrame = rightMousePressed;

		DirectX::XMVECTOR directionVec = camera.CalculateDirection();

		//restrict movement to the XZ plane
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

		camera.SetPosition(position);
	}
}