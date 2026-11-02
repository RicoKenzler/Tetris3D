export module GameLogic:GameObject;

import Core;

export namespace game
{
	class GameObject
	{
	public:
		GameObject() = default;
		GameObject(int id) : m_id(id) {}

		[[nodiscard]] int GetId() const { return m_id; }

		[[nodiscard]] const core::TransformData& GetTransformData() const { return m_transformData; }
		[[nodiscard]] core::TransformData& GetTransformData() { return m_transformData; }

	private:
		int m_id = 0;
		core::TransformData m_transformData;
	};
}