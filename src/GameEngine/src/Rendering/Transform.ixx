module;

#include <DirectXMath.h>
#include <d3d11.h>

export module Engine.Rendering:Transform;

export namespace engine
{
	class Transform
	{
	public:
		Transform() = default;
		~Transform() = default;

		[[nodiscard]] DirectX::XMFLOAT3 GetPosition() const { return m_position; }
		void SetPosition(DirectX::XMFLOAT3 value) { m_position = value; }

		[[nodiscard]] DirectX::XMFLOAT3 GetRotation() const { return m_rotation; }
		void SetRotation(DirectX::XMFLOAT3 value) { m_rotation = value; }

		[[nodiscard]] DirectX::XMFLOAT3 GetScale() const { return m_scale; }
		void SetScale(DirectX::XMFLOAT3 value) { m_scale = value; }

		[[nodiscard]] DirectX::XMMATRIX CalculateWorldMatrix() const;

	private:
		DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 m_scale = { 1.0f, 1.0f, 1.0f };
	};
}