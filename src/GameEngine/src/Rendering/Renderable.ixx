export module Engine.Rendering:Renderable;

import Core;
import :Mesh;
import :Texture;
import :Transform;

export namespace engine
{
	class Renderable
	{
	public:
		void Init(int id, Mesh* mesh, Texture* texture);

		void SetTransformData(const core::TransformData& transformData);

		[[nodiscard]] int GetId() const { return m_id; }

		[[nodiscard]] const Mesh* GetMesh() const { return m_mesh; }

		[[nodiscard]] const Texture* GetTexture() const { return m_texture; }
		[[nodiscard]] Texture* GetTexture() { return m_texture; }

		[[nodiscard]] const Transform& GetTransform() const { return m_transform; }
		[[nodiscard]] Transform& GetTransform() { return m_transform; }

	private:
		int m_id = 0;
		Mesh* m_mesh = nullptr;
		Texture* m_texture = nullptr;
		Transform m_transform;
	};
}