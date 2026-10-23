module;

#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

export module RenderEngine.Rendering:Engine;

using Microsoft::WRL::ComPtr;

export namespace RenderEngine
{
	class Engine
	{
	public:
		Engine() = default;

		void Start(HINSTANCE instance, int showCommand);
		int Run();

	private:
		static void CheckHR(HRESULT result, const char* message);
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		void CreateTriangleResources();
		void RenderFrame();

		ComPtr<ID3D11DeviceContext> m_context;
		ComPtr<IDXGISwapChain1> m_swapChain;
		ComPtr<ID3D11RenderTargetView> m_renderTarget;
		ComPtr<ID3D11Device> m_device;

		ComPtr<ID3D11VertexShader> m_vertexShader;
		ComPtr<ID3D11PixelShader> m_pixelShader;
		ComPtr<ID3D11InputLayout> m_inputLayout;
		ComPtr<ID3D11Buffer> m_vertexBuffer;
	};
	
}