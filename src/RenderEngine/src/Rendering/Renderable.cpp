module;

#include <DirectXMath.h>

module RenderEngine.Rendering;

import :Renderable;
import :Mesh;

namespace RenderEngine
{
	void Renderable::Init(Mesh* mesh)
	{
		m_mesh = mesh;
	}
}