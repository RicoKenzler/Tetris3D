module;

#include <DirectXMath.h>

module RenderEngine.Rendering;

import :Renderable;
import :Mesh;

namespace RenderEngine
{
	void Renderable::Init(Mesh* const mesh, Texture* const texture)
	{
		m_mesh = mesh;
		m_texture = texture;
	}
}