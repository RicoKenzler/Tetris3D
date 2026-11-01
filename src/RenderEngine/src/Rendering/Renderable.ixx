export module RenderEngine.Rendering:Renderable;

import :Mesh;
import :Texture;
import :Transform;

export namespace RenderEngine
{
	class Renderable
	{
	public:
		void Init(Mesh* mesh, Texture* texture);

		[[nodiscard]] const Mesh* GetMesh() const { return m_mesh; }

		[[nodiscard]] const Texture* GetTexture() const { return m_texture; }
		[[nodiscard]] Texture* GetTexture() { return m_texture; }

		[[nodiscard]] const Transform& GetTransform() const { return m_transform; }
		[[nodiscard]] Transform& GetTransform() { return m_transform; }

	private:
		Mesh* m_mesh = nullptr;
		Texture* m_texture = nullptr;
		Transform m_transform;
	};
}