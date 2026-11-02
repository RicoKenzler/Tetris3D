module;

#include <DirectXMath.h>

module Engine.Rendering;

import :Renderable;
import :Mesh;

namespace engine
{
	void Renderable::Init(int id, Mesh* const mesh, Texture* const texture)
	{
		m_id = id;
		m_mesh = mesh;
		m_texture = texture;
	}

	void Renderable::SetTransformData(const core::TransformData& transformData)
	{
		m_transform.SetPosition(DirectX::XMFLOAT3(transformData.position.x, transformData.position.y, transformData.position.z));
		m_transform.SetRotation(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(transformData.rotation.x), DirectX::XMConvertToRadians(transformData.rotation.y), DirectX::XMConvertToRadians(transformData.rotation.z)));
		m_transform.SetScale(DirectX::XMFLOAT3(transformData.scale.x, transformData.scale.y, transformData.scale.z));
	}
}