module;

#include <DirectXMath.h>

module Engine.Rendering;

import :Renderable;
import :Mesh;

namespace engine
{
	void Renderable::Init(Mesh* const mesh, Texture* const texture)
	{
		m_mesh = mesh;
		m_texture = texture;
	}
}