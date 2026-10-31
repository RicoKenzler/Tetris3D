export module RenderEngine.Rendering:Renderable;

import :Mesh;
import :Transform;

export namespace RenderEngine
{
	class Renderable
	{
	public:
		void Init(Mesh* mesh);

		[[nodiscard]] const Mesh* GetMesh() const { return m_mesh; }

		[[nodiscard]] const Transform& GetTransform() const { return m_transform; }
		[[nodiscard]] Transform& GetTransform() { return m_transform; }

	private:
		Mesh* m_mesh = nullptr;
		Transform m_transform;
	};
}