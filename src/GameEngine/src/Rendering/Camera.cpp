module;

#include <Windows.h>

#include <DirectXMath.h>

module Engine.Rendering;

import :Camera;

namespace engine
{
	DirectX::XMMATRIX Camera::CalculateView() const
	{
		const DirectX::XMVECTOR cameraPosition = DirectX::XMLoadFloat3(&m_position);
		const DirectX::XMVECTOR cameraDirection = CalculateDirection();
		const DirectX::XMVECTOR cameraUp = DirectX::XMLoadFloat3(&m_up);
		const DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(cameraPosition, cameraDirection, cameraUp);
		return view;
	}

	DirectX::XMVECTOR Camera::CalculateDirection() const
	{
		const DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationX(-m_pitch) * DirectX::XMMatrixRotationY(m_yaw);
		const DirectX::XMVECTOR direction = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_forward), rotationMatrix);
		return DirectX::XMVector3Normalize(direction);
	}
}