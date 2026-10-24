module;

#include <DirectXMath.h>

export module RenderEngine.Rendering:Camera;

export namespace RenderEngine
{
	class Camera
	{
	public:
		Camera() = default;
		
		[[nodiscard]] DirectX::XMMATRIX CalculateView() const;

		[[nodiscard]] DirectX::XMVECTOR CalculateDirection() const;
		
		[[nodiscard]] DirectX::XMFLOAT3 GetPosition() const { return m_position; }
		void SetPosition(const DirectX::XMFLOAT3& position) { m_position = position; }
		
		[[nodiscard]] float GetYaw() const { return m_yaw; }
		void SetYaw(float value) { m_yaw = value; }

		[[nodiscard]] float GetPitch() const { return m_pitch; }
		void SetPitch(float value) { m_pitch = value; }
		
		[[nodiscard]] DirectX::XMFLOAT3 GetUp() const { return m_up; }


	private:
		DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, -5.0f };
		DirectX::XMFLOAT3 m_up = { 0.0f, 1.0f, 0.0f };
		DirectX::XMFLOAT3 m_forward = { 0.0f, 0.0f, 1.0f };
		float m_yaw = 0.0f;
		float m_pitch = 0.0f;
	};
	
}