module GameLogic;

import :GameObject;

namespace game
{
	const core::Vector3& GameObject::GetPosition() const
	{
		return m_transformData.position;
	}

	void GameObject::SetPosition(const core::Vector3& value)
	{
		m_transformData.position = value;
		m_isDirty = true;
	}

	const core::Vector3& GameObject::GetRotation() const
	{
		return m_transformData.rotation;
	}
	
	void GameObject::SetRotation(const core::Vector3& value)
	{
		m_transformData.rotation = value;
		m_isDirty = true;
	}
	
	const core::Vector3& GameObject::GetScale() const
	{
		return m_transformData.scale;
	}
	
	void GameObject::SetScale(const core::Vector3& value)
	{
		m_transformData.scale = value;
		m_isDirty = true;
	}
	
	bool GameObject::GetIsDirty() const
	{
		return m_isDirty;
	}
	
	void GameObject::SetIsDirty(bool value)
	{
		m_isDirty = value;
	}
}