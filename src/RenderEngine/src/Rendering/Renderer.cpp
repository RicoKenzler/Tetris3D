module;

#include <DirectXMath.h>
#include <Windows.h>
#include <cstdint>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#include <stdexcept>
#include <string>
#include <cstddef>
#include <vector>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

module RenderEngine.Rendering;

import :Renderer;
import :Camera;
import :Mesh;
import :Vertex;

using Microsoft::WRL::ComPtr;

namespace RenderEngine
{
	struct Pixel
	{
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
		std::uint8_t a;
	};

	struct TransformConstants
	{
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4X4 normalMatrix;
	};

	static_assert(sizeof(TransformConstants) % 16 == 0);


	void Renderer::Initialize(HWND hwnd, UINT width, UINT height)
	{
		m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		
		CreateDevice();
		CreateSwapChain(hwnd, width, height);
		CreateRenderTarget(width, height);
		CreateDepthBuffer(width, height);
		CreateSampler();
		CreateCubeResources();
	}

	void Renderer::Render(const float elapsedSeconds, const Camera& camera)
	{
		using namespace DirectX;

		// Build the world matrix.
		// Rotate the cube at 45 degrees per second.
		constexpr float rotationSpeed = DirectX::XMConvertToRadians(45.0f);
		const float angle = elapsedSeconds * rotationSpeed;
		

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


		// Bind the shaders.
		m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

		m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

		const XMMATRIX view = camera.CalculateView();
		const XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), m_aspectRatio, 0.1f, 100.0f);
		const XMMATRIX viewprojection = view * projection;

		m_cube2.GetTransform().SetRotation({ angle, 0, 0 });
		m_cube3.GetTransform().SetRotation({ 0, 0, angle });
		m_pyramid.GetTransform().SetRotation({ 0, angle, 0});

		RenderObject(m_cube1, viewprojection);
		RenderObject(m_cube2, viewprojection);
		RenderObject(m_cube3, viewprojection);
		RenderObject(m_pyramid, viewprojection);

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

	void Renderer::CreateSampler()
	{
		D3D11_SAMPLER_DESC samplerDesc{};

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT result = m_device->CreateSamplerState(
			&samplerDesc,
			m_samplerState.GetAddressOf());
	}

	void Renderer::CreateCubeResources()
	{
		//Create a texture
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = 64;
		desc.Height = 64;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

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

		D3D11_SUBRESOURCE_DATA initialData{};
		initialData.pSysMem = pixels.data();
		initialData.SysMemPitch = 64 * sizeof(Pixel);

		HRESULT result = m_device->CreateTexture2D(&desc, &initialData, m_checkerTexture.GetAddressOf());
		CheckHR(result, "CreateTexture failed");

		result = m_device->CreateShaderResourceView(m_checkerTexture.Get(), nullptr, m_checkerTextureResourceView.GetAddressOf());
		CheckHR(result, "CreateShaderResourceView failed");

		m_cubeMesh.InitCube(*m_device.Get());
		m_pyramidMesh.InitPyramid(*m_device.Get());

		m_cube1.Init(&m_cubeMesh);
		m_cube2.Init(&m_cubeMesh);
		m_cube3.Init(&m_cubeMesh);

		m_cube1.GetTransform().SetPosition({ -3, 0, 0 });
		m_cube1.GetTransform().SetScale({ 0.75f, 0.75f, 0.75f });

		m_cube3.GetTransform().SetPosition({ 3, 0, 0 });
		m_cube3.GetTransform().SetScale({ 0.75f, 0.75f, 0.75f });

		m_pyramid.Init(&m_pyramidMesh);
		m_pyramid.GetTransform().SetPosition({ 6, 0, 0 });


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
				offsetof(Vertex, x),
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				offsetof(Vertex, r),
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"NORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				offsetof(Vertex, nx),
				D3D11_INPUT_PER_VERTEX_DATA
			},
			{
				"TEXCOORD",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				offsetof(Vertex, u),
				D3D11_INPUT_PER_VERTEX_DATA
			}
		};

		result = m_device->CreateInputLayout(
			inputElements,
			sizeof(inputElements) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			vertexBytecode->GetBufferPointer(),
			vertexBytecode->GetBufferSize(),
			m_inputLayout.GetAddressOf()
		);
		DWORD error = GetLastError();
		CheckHR(result, "CreateInputLayout failed");


		// Create a constant buffer for the WVP matrix.
		D3D11_BUFFER_DESC constantBufferDesc{};
		constantBufferDesc.ByteWidth = sizeof(TransformConstants);
		constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		result = m_device->CreateBuffer(&constantBufferDesc, nullptr, m_transformBuffer.GetAddressOf());
		CheckHR(result, "Create transform buffer failed");
	}

	void Renderer::RenderObject(const Renderable& renderable, const DirectX::XMMATRIX& viewprojection)
	{
		using namespace DirectX;

		// Configure the input assembler.
		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;

		ID3D11Buffer* vertexBuffer = renderable.GetMesh()->GetVertexBuffer();

		m_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

		m_context->IASetIndexBuffer(renderable.GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R16_UINT, 0);

		m_context->IASetInputLayout(m_inputLayout.Get());

		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_context->PSSetShaderResources(0, 1, m_checkerTextureResourceView.GetAddressOf());
		m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

		const XMMATRIX world = renderable.GetTransform().CalculateWorldMatrix();

		// Combine the transformation matrices.
		const XMMATRIX wvp = world * viewprojection;
		const XMMATRIX normalMatrix = DirectX::XMMatrixInverse(nullptr, world);

		TransformConstants constants{};

		XMStoreFloat4x4(&constants.worldViewProjection, XMMatrixTranspose(wvp));
		XMStoreFloat4x4(&constants.normalMatrix, normalMatrix);

		// Upload the transformation to the GPU.
		m_context->UpdateSubresource(m_transformBuffer.Get(), 0, nullptr, &constants, 0, 0);

		ID3D11Buffer* constantBuffer = m_transformBuffer.Get();

		m_context->VSSetConstantBuffers(0, 1, &constantBuffer);

		// Draw the triangle.
		m_context->DrawIndexed(renderable.GetMesh()->GetIndexCount(), 0, 0);
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

	void Renderer::CheckHR(HRESULT result, const char* message)
	{
		if (FAILED(result))
			throw std::runtime_error(message);
	}
}