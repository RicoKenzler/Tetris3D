module;

#include <string_view>
#include <WICTextureLoader.h>
#include <vector>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <stdexcept>
#include <wrl/client.h>

module RenderEngine.Rendering;

import :Texture;

namespace RenderEngine
{
	void Texture::CreateCheckerTexture(ID3D11Device& device, ID3D11DeviceContext& context)
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = 64;
		desc.Height = 64;
		desc.MipLevels = 0;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		std::vector<Pixel> pixels;
		pixels.reserve(64 * 64);
		for (int i = 0; i < 64 * 64; ++i)
		{
			int x = i % 64;
			int y = i / 64;
			int z = (x / 8 + y / 8) % 2;
			if (z == 0)
			{
				pixels.emplace_back(0, 0, 0, 255);
			}
			else
			{
				pixels.emplace_back(255, 255, 255, 255);
			}
		}


		ID3D11Texture2D* checkerTexture;

		HRESULT result = device.CreateTexture2D(&desc, nullptr, &checkerTexture);
		CheckHR(result, "CreateTexture failed");

		context.UpdateSubresource(
			checkerTexture,
			0,
			nullptr,
			pixels.data(),
			64 * sizeof(Pixel),
			0
		);

		result = device.CreateShaderResourceView(checkerTexture, nullptr, m_textureResourceView.GetAddressOf());
		CheckHR(result, "CreateShaderResourceView failed");

		context.GenerateMips(m_textureResourceView.Get());
	}

	void Texture::LoadFromFile(ID3D11Device& device, ID3D11DeviceContext& context, std::wstring_view path)
	{
		HRESULT result = DirectX::CreateWICTextureFromFile(
			&device,
			&context,
			path.data(),
			nullptr,
			m_textureResourceView.GetAddressOf()
		);
		CheckHR(result, "Loading of texture failed");
	}

	void Texture::CheckHR(HRESULT result, const char* message)
	{
		if (FAILED(result))
			throw std::runtime_error(message);
	}
}