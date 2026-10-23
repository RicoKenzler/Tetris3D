module;

#include <Windows.h>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

#include <stdexcept>

#include <d3dcompiler.h>
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

module RenderEngine.Rendering;

import :Engine;

using Microsoft::WRL::ComPtr;

namespace RenderEngine
{
	constexpr UINT WindowWidth = 1280;
	constexpr UINT WindowHeight = 720;

	struct Vertex
	{
		float x, y, z;
		float r, g, b;
	};


	constexpr char TriangleShaderSource[] = R"(
    struct VSInput
    {
        float3 position : POSITION;
        float3 color    : COLOR;
    };

    struct VSOutput
    {
        float4 position : SV_POSITION;
        float3 color    : COLOR;
    };

    VSOutput VSMain(VSInput input)
    {
        VSOutput output;

        output.position = float4(input.position, 1.0f);
        output.color = input.color;

        return output;
    }

    float4 PSMain(VSOutput input) : SV_TARGET
    {
        return float4(input.color, 1.0f);
    })";


	void Engine::Start(HINSTANCE instance, int showCommand)
	{
		// 1. register window class

		WNDCLASSW windowClass{};
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = instance;
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.lpszClassName = L"MiniRenderWindow";

		if (!RegisterClassW(&windowClass))
			throw std::runtime_error("RegisterClass failed");

		// TODO: add resize functionality later
		const DWORD windowStyle =
			WS_OVERLAPPED |
			WS_CAPTION |
			WS_SYSMENU |
			WS_MINIMIZEBOX;

		RECT windowRect{
			0, 0,
			static_cast<LONG>(WindowWidth),
			static_cast<LONG>(WindowHeight)
		};

		if (!AdjustWindowRect(&windowRect, windowStyle, FALSE))
			throw std::runtime_error("AdjustWindowRect failed");

		HWND hwnd = CreateWindowExW(
			0,
			windowClass.lpszClassName,
			L"Mini Render Engine",
			windowStyle,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,
			nullptr,
			instance,
			nullptr
		);

		if (!hwnd)
			throw std::runtime_error("CreateWindow failed");


		// 2. create direct3D device and device context

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


		// 3. get DXGI Factory

		ComPtr<IDXGIDevice> dxgiDevice;
		result = m_device.As(&dxgiDevice);
		CheckHR(result, "Query IDXGIDevice failed");

		ComPtr<IDXGIAdapter> adapter;
		result = dxgiDevice->GetAdapter(adapter.GetAddressOf());
		CheckHR(result,	"GetAdapter failed");

		ComPtr<IDXGIFactory2> factory;
		result = adapter->GetParent(IID_PPV_ARGS(factory.GetAddressOf()));
		CheckHR(result, "GetParent failed");


		// 4. create swap chain

		DXGI_SWAP_CHAIN_DESC1 swapDesc{};
		swapDesc.Width = WindowWidth;
		swapDesc.Height = WindowHeight;
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


		// 5. create render target view

		ComPtr<ID3D11Texture2D> backBuffer;

		result = m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		CheckHR(result, "GetBuffer failed");

		result = m_device->CreateRenderTargetView(
			backBuffer.Get(),
			nullptr,
			m_renderTarget.GetAddressOf()
		);
		CheckHR(result, "CreateRenderTargetView failed");

		backBuffer.Reset();


		// 6. configure viewport

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(WindowWidth);
		viewport.Height = static_cast<float>(WindowHeight);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		m_context->RSSetViewports(1, &viewport);


		// 7. create our triangle resources (vertex buffer, shaders, input layout)
		CreateTriangleResources();


		// 8. show window

		ShowWindow(hwnd, showCommand);
	}
	
	int Engine::Run()
	{
		MSG msg{};
		bool running = true;

		while (running)
		{
			while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					running = false;
					break;
				}

				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}

			if (!running)
				break;

			RenderFrame();
		}

		m_context->ClearState();

		return static_cast<int>(msg.wParam);
	}

	void Engine::CheckHR(HRESULT result, const char* message)
	{
		if (FAILED(result))
			throw std::runtime_error(message);
	}

	LRESULT CALLBACK Engine::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_DESTROY)
		{
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProcW(hwnd, message, wParam, lParam);
	}

	void Engine::CreateTriangleResources()
	{
		// 1. Define the triangle vertices.
		constexpr Vertex vertices[] =
		{
			{ -0.6f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f },
			{ 0.0f,  0.6f, 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.6f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f }
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


		// 3. Compile the HLSL shaders.
		auto compileShader = [](const char* entry, const char* target)
		{
			ComPtr<ID3DBlob> bytecode;
			ComPtr<ID3DBlob> errors;

			HRESULT result = D3DCompile(
				TriangleShaderSource,
				sizeof(TriangleShaderSource) - 1,
				"TriangleShader",
				nullptr,
				nullptr,
				entry,
				target,
				D3DCOMPILE_ENABLE_STRICTNESS,
				0,
				bytecode.GetAddressOf(),
				errors.GetAddressOf()
			);

			if (FAILED(result))
			{
				if (errors)
				{
					throw std::runtime_error(
						std::string(
							static_cast<const char*>(errors->GetBufferPointer()),
							errors->GetBufferSize()
						)
					);
				}

				throw std::runtime_error("Shader compilation failed");
			}

			return bytecode;
		};

		ComPtr<ID3DBlob> vertexBytecode = compileShader("VSMain", "vs_5_0");
		ComPtr<ID3DBlob> pixelBytecode = compileShader("PSMain", "ps_5_0");


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

	void Engine::RenderFrame()
	{
		// 1. Bind the render target.
		ID3D11RenderTargetView* renderTargets[] = {
			m_renderTarget.Get()
		};

		m_context->OMSetRenderTargets(
			1,
			renderTargets,
			nullptr
		);

		// 2. Clear the back buffer.
		constexpr float clearColor[4] = {
			0.05f, 0.10f, 0.18f, 1.0f
		};

		m_context->ClearRenderTargetView(
			m_renderTarget.Get(),
			clearColor
		);

		// 3. Configure the input assembler.
		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;

		ID3D11Buffer* vertexBuffers[] = {
			m_vertexBuffer.Get()
		};

		m_context->IASetVertexBuffers(
			0,
			1,
			vertexBuffers,
			&stride,
			&offset
		);

		m_context->IASetInputLayout(m_inputLayout.Get());

		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 4. Bind the shaders.
		m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

		m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

		// 5. Draw the triangle.
		m_context->Draw(3, 0);

		// 6. Present the frame.
		HRESULT result = m_swapChain->Present(1, 0);
		CheckHR(result, "Present failed");
	}
}