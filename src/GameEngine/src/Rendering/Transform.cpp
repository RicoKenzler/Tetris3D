module;

#include <DirectXMath.h>

module Engine.Rendering;

import :Transform;

namespace engine
{
	DirectX::XMMATRIX Transform::CalculateWorldMatrix() const
	{
		const DirectX::XMMATRIX world = 
			DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * 
			DirectX::XMMatrixRotationX(m_rotation.x) * DirectX::XMMatrixRotationY(m_rotation.y) * DirectX::XMMatrixRotationZ(m_rotation.z) * 
			DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
		return world;
	}
}