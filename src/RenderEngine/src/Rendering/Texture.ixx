module;

#include <string_view>
#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

export module RenderEngine.Rendering:Texture;

using Microsoft::WRL::ComPtr;

export namespace RenderEngine
{
	class Texture
	{
	public:
		void CreateCheckerTexture(ID3D11Device& device, ID3D11DeviceContext& context);
		void LoadFromFile(ID3D11Device& device, ID3D11DeviceContext& context, std::wstring_view path);

		[[nodiscard]] ID3D11ShaderResourceView* GetTextureResourceView() const { return m_textureResourceView.Get(); }

	private:
		static void CheckHR(HRESULT result, const char* message);

		ComPtr<ID3D11ShaderResourceView> m_textureResourceView;
	};
	
}