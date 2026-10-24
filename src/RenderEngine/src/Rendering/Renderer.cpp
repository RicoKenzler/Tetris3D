module;

#include <DirectXMath.h>
#include <Windows.h>
#include <cstdint>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#include <stdexcept>
#include <string>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

module RenderEngine.Rendering;

import :Renderer;
import :Camera;

using Microsoft::WRL::ComPtr;

namespace RenderEngine
{
	struct Vertex
	{
		float x, y, z;
		float r, g, b;
	};

	struct TransformConstants
	{
		DirectX::XMFLOAT4X4 worldViewProjection;
	};

	static_assert(sizeof(TransformConstants) % 16 == 0);


	void Renderer::Initialize(HWND hwnd, UINT width, UINT height)
	{
		m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		
		CreateDevice();
		CreateSwapChain(hwnd, width, height);
		CreateRenderTarget(width, height);
		CreateDepthBuffer(width, height);
		CreateCubeResources();
	}

	void Renderer::Render(const float elapsedSeconds, const Camera& camera)
	{
		using namespace DirectX;

		// Build the world matrix.
		// Rotate the cube at 45 degrees per second.
		constexpr float rotationSpeed = DirectX::XMConvertToRadians(45.0f);
		const float angle = elapsedSeconds * rotationSpeed;
		const XMMATRIX world = XMMatrixRotationX(angle);

		const XMMATRIX view = camera.CalculateView();

		// Build the perspective projection.
		const XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), m_aspectRatio, 0.1f, 100.0f);

		// Combine the transformation matrices.
		const XMMATRIX wvp = world * view * projection;

		TransformConstants constants{};

		XMStoreFloat4x4(&constants.worldViewProjection, XMMatrixTranspose(wvp));

		// Upload the transformation to the GPU.
		m_context->UpdateSubresource(m_transformBuffer.Get(), 0, nullptr, &constants, 0, 0);


		// Bind the render target.
		ID3D11RenderTargetView* renderTargets[] = { m_renderTarget.Get() };

		m_context->OMSetRenderTargets(1, renderTargets, m_depthStencilView.Get());

		// Clear the back buffer.
		constexpr float clearColor[4] =
		{
			0.05f, 0.10f, 0.18f, 1.0f
		};

		m_context->ClearRenderTargetView(m_renderTarget.Get(), clearColor);

		m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		// Configure the input assembler.
		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;

		ID3D11Buffer* vertexBuffer = m_vertexBuffer.Get();

		m_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

		m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

		m_context->IASetInputLayout(m_inputLayout.Get());

		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Bind the shaders.
		m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

		m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

		ID3D11Buffer* constantBuffer = m_transformBuffer.Get();

		m_context->VSSetConstantBuffers(0, 1, &constantBuffer);

		// Draw the triangle.
		m_context->DrawIndexed(36, 0, 0);

		// Present the frame.
		HRESULT result = m_swapChain->Present(1, 0);
		CheckHR(result, "Present failed");
	}

	void Renderer::ShutDown()
	{
		m_context->ClearState();
	}

	void Renderer::CreateDevice()
	{
		const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		HRESULT result = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			&featureLevel,
			1,
			D3D11_SDK_VERSION,
			m_device.GetAddressOf(),
			nullptr,
			m_context.GetAddressOf()
		);
		CheckHR(result, "D3D11CreateDevice failed");
	}

	void Renderer::CreateSwapChain(HWND hwnd, UINT width, UINT height)
	{
		ComPtr<IDXGIDevice> dxgiDevice;
		HRESULT result = m_device.As(&dxgiDevice);
		CheckHR(result, "Query IDXGIDevice failed");

		ComPtr<IDXGIAdapter> adapter;
		result = dxgiDevice->GetAdapter(adapter.GetAddressOf());
		CheckHR(result, "GetAdapter failed");

		ComPtr<IDXGIFactory2> factory;
		result = adapter->GetParent(IID_PPV_ARGS(factory.GetAddressOf()));
		CheckHR(result, "GetParent failed");

		DXGI_SWAP_CHAIN_DESC1 swapDesc{};
		swapDesc.Width = width;
		swapDesc.Height = height;
		swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.BufferCount = 2;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		result = factory->CreateSwapChainForHwnd(
			m_device.Get(),
			hwnd,
			&swapDesc,
			nullptr,
			nullptr,
			m_swapChain.GetAddressOf()
		);
		CheckHR(result, "CreateSwapChainForHwnd failed");
	}

	void Renderer::CreateRenderTarget(UINT width, UINT height)
	{
		ComPtr<ID3D11Texture2D> backBuffer;

		HRESULT result = m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		CheckHR(result, "GetBuffer failed");

		result = m_device->CreateRenderTargetView(
			backBuffer.Get(),
			nullptr,
			m_renderTarget.GetAddressOf()
		);
		CheckHR(result, "CreateRenderTargetView failed");

		backBuffer.Reset();

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		m_context->RSSetViewports(1, &viewport);
	}

	void Renderer::CreateDepthBuffer(UINT width, UINT height)
	{
		// Describe the depth-stencil texture.
		D3D11_TEXTURE2D_DESC depthDesc{};

		depthDesc.Width = width;
		depthDesc.Height = height;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		// Create the depth-stencil texture.
		ComPtr<ID3D11Texture2D> depthTexture;

		HRESULT result = m_device->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
		CheckHR(result, "Create depth texture failed");
		
		// Create a view for depth testing.
		result = m_device->CreateDepthStencilView(depthTexture.Get(), nullptr, m_depthStencilView.GetAddressOf());
		CheckHR(result, "Create depth stencil view failed");
	}

	void Renderer::CreateCubeResources()
	{
		// Eight corners of a cube centered at the origin.
		constexpr Vertex vertices[] =
		{
			{ -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f }, // 0
			{ -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f }, // 1
			{ 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f }, // 2
			{ 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f }, // 3

			{ -1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 1.0f }, // 4
			{ -1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f }, // 5
			{ 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f }, // 6
			{ 1.0f, -1.0f,  1.0f, 1.0f, 0.5f, 0.0f }  // 7
		};

		// Each group of three indices defines one triangle.
		// The winding order matches the default D3D11 rasterizer.
		constexpr std::uint16_t indices[] =
		{
			// Front (-Z)
			0, 1, 2,
			0, 2, 3,

			// Back (+Z)
			4, 6, 5,
			4, 7, 6,

			// Left (-X)
			0, 4, 5,
			0, 5, 1,

			// Right (+X)
			3, 2, 6,
			3, 6, 7,

			// Top (+Y)
			1, 5, 6,
			1, 6, 2,

			// Bottom (-Y)
			0, 3, 7,
			0, 7, 4
		};


		// Create the vertex buffer.
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(vertices));
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initialData{};
		initialData.pSysMem = vertices;

		HRESULT result = m_device->CreateBuffer(
			&bufferDesc,
			&initialData,
			m_vertexBuffer.GetAddressOf()
		);
		CheckHR(result, "CreateBuffer failed");


		// Describe the index buffer.
		D3D11_BUFFER_DESC indexDesc{};
		indexDesc.ByteWidth = static_cast<UINT>(sizeof(indices));
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		// Provide the initial index data.
		D3D11_SUBRESOURCE_DATA indexData{};
		indexData.pSysMem = indices;

		// Create the GPU index buffer.
		result = m_device->CreateBuffer(
			&indexDesc,
			&indexData,
			m_indexBuffer.GetAddressOf()
		);
		CheckHR(result, "Create index buffer failed");




		// 3. Compile the HLSL shaders.
		const wchar_t* shaderPath = L"src\\RenderEngine\\src\\Shaders\\Cube.hlsl";
		ComPtr<ID3DBlob> vertexBytecode = CompileShader(shaderPath, "VSMain", "vs_5_0");
		ComPtr<ID3DBlob> pixelBytecode = CompileShader(shaderPath, "PSMain", "ps_5_0");


		// 4. Create the Direct3D shader objects.
		result = m_device->CreateVertexShader(
			vertexBytecode->GetBufferPointer(),
			vertexBytecode->GetBufferSize(),
			nullptr,
			m_vertexShader.GetAddressOf()
		);
		CheckHR(result, "CreateVertexShader failed");

		result = m_device->CreatePixelShader(
			pixelBytecode->GetBufferPointer(),
			pixelBytecode->GetBufferSize(),
			nullptr,
			m_pixelShader.GetAddressOf()
		);
		CheckHR(result, "CreatePixelShader failed");


		// 5. Describe the vertex memory layout.
		const D3D11_INPUT_ELEMENT_DESC inputElements[] =
		{
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				0,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				12,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			}
		};

		result = m_device->CreateInputLayout(
			inputElements,
			2,
			vertexBytecode->GetBufferPointer(),
			vertexBytecode->GetBufferSize(),
			m_inputLayout.GetAddressOf()
		);
		CheckHR(result, "CreateInputLayout failed");


		// Create a constant buffer for the WVP matrix.
		D3D11_BUFFER_DESC constantBufferDesc{};
		constantBufferDesc.ByteWidth = sizeof(TransformConstants);
		constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		result = m_device->CreateBuffer(&constantBufferDesc, nullptr, m_transformBuffer.GetAddressOf());
		CheckHR(result, "Create transform buffer failed");
	}

	void Renderer::CheckHR(HRESULT result, const char* message)
	{
		if (FAILED(result))
			throw std::runtime_error(message);
	}

	ComPtr<ID3DBlob> Renderer::CompileShader(const wchar_t* shaderPath, const char* entry, const char* target) const
	{
		ComPtr<ID3DBlob> bytecode;
		ComPtr<ID3DBlob> errors;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		const HRESULT result = D3DCompileFromFile(
			shaderPath,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entry,
			target,
			flags,
			0,
			bytecode.GetAddressOf(),
			errors.GetAddressOf()
		);

		if (FAILED(result))
		{
			std::string message = "Failed to compile Shader";

			if (errors)
			{
				message += ":\n";
				message.append(
					static_cast<const char*>(errors->GetBufferPointer()),
					errors->GetBufferSize()
				);
			}
			else
			{
				message += " (HRESULT: ";
				message += std::to_string(static_cast<unsigned int>(result));
				message += ")";
			}

			throw std::runtime_error(message);
		}

		return bytecode;
	}
}