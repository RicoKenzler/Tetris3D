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

		[[nodiscard]] const core::Vector3& GetPosition() const;
		void SetPosition(const core::Vector3& value);
		[[nodiscard]] const core::Vector3& GetRotation() const;
		void SetRotation(const core::Vector3& value);
		[[nodiscard]] const core::Vector3& GetScale() const;
		void SetScale(const core::Vector3& value);

		[[nodiscard]] bool GetIsDirty() const;
		void SetIsDirty(bool value);

		[[nodiscard]] const core::TransformData& GetTransformData() const { return m_transformData; }

	private:
		int m_id = 0;
		core::TransformData m_transformData;
		bool m_isDirty = false;
	};
	
}