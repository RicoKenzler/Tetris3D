module;

#include <Windows.h>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <cstdint>

#include <stdexcept>

#include <d3dcompiler.h>
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

module RenderEngine.Rendering;

import :Renderer;

using Microsoft::WRL::ComPtr;

namespace RenderEngine
{
	struct Vertex
	{
		float x, y, z;
		float r, g, b;
	};

	void Renderer::Initialize(HWND hwnd, UINT width, UINT height)
	{
		CreateDevice();
		CreateSwapChain(hwnd, width, height);
		CreateRenderTarget(width, height);
		CreateRectangleResources();
	}

	void Renderer::Render()
	{
		// 1. Bind the render target.
		ID3D11RenderTargetView* renderTargets[] = { m_renderTarget.Get() };

		m_context->OMSetRenderTargets(1, renderTargets, nullptr);

		// 2. Clear the back buffer.
		constexpr float clearColor[4] =
		{
			0.05f, 0.10f, 0.18f, 1.0f
		};

		m_context->ClearRenderTargetView(m_renderTarget.Get(), clearColor);

		// 3. Configure the input assembler.
		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;

		ID3D11Buffer* vertexBuffers[] =
		{
			m_vertexBuffer.Get()
		};

		m_context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

		m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

		m_context->IASetInputLayout(m_inputLayout.Get());

		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 4. Bind the shaders.
		m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

		m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

		// 5. Draw the triangle.
		m_context->DrawIndexed(6, 0, 0);

		// 6. Present the frame.
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

	void Renderer::CreateRectangleResources()
	{
		// 1. Define the rectangle vertices.
		constexpr Vertex vertices[] =
		{
			// Bottom-left
			{ -0.65f, -0.45f, 0.0f, 1.0f, 0.0f, 0.0f },

			// Top-left
			{ -0.65f,  0.45f, 0.0f, 0.0f, 1.0f, 0.0f },

			// Top-right
			{ 0.65f,  0.45f, 0.0f, 0.0f, 0.0f, 1.0f },

			// Bottom-right
			{ 0.65f, -0.45f, 0.0f, 1.0f, 1.0f, 0.0f }
		};


		// 2. Create the vertex buffer.
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



		// Define two triangles using shared vertices.
		constexpr std::uint16_t indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

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
		ComPtr<ID3DBlob> vertexBytecode = CompileShader("VSMain", "vs_5_0");
		ComPtr<ID3DBlob> pixelBytecode = CompileShader("PSMain", "ps_5_0");


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
	}

	void Renderer::CheckHR(HRESULT result, const char* message)
	{
		if (FAILED(result))
			throw std::runtime_error(message);
	}

	ComPtr<ID3DBlob> Renderer::CompileShader(const char* entry, const char* target) const
	{
		ComPtr<ID3DBlob> bytecode;
		ComPtr<ID3DBlob> errors;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		const HRESULT result = D3DCompileFromFile(
			L"src\\RenderEngine\\src\\Shaders\\Triangle.hlsl",
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
			std::string message = "Failed to compile Shaders/Triangle.hlsl";

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