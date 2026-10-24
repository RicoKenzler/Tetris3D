module;

#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

export module RenderEngine.Rendering:Renderer;

using Microsoft::WRL::ComPtr;

export namespace RenderEngine
{
    class Renderer
    {
    public:
        void Initialize(HWND hwnd, UINT width, UINT height);
        void Render();
		void ShutDown();

    private:
        void CreateDevice();
        void CreateSwapChain(HWND hwnd, UINT width, UINT height);
        void CreateRenderTarget(UINT width, UINT height);
        void CreateDepthBuffer(UINT width, UINT height);
        void CreateCubeResources();

        static void CheckHR(HRESULT result, const char* message);

        [[nodiscard]] ComPtr<ID3DBlob> CompileShader(const wchar_t* shaderPath, const char* entry, const char* target) const;


        // Direct3D resources are owned by the renderer.
        ComPtr<ID3D11Device> m_device;
        ComPtr<ID3D11DeviceContext> m_context;
        ComPtr<IDXGISwapChain1> m_swapChain;
        ComPtr<ID3D11RenderTargetView> m_renderTarget;

        ComPtr<ID3D11VertexShader> m_vertexShader;
        ComPtr<ID3D11PixelShader> m_pixelShader;
        ComPtr<ID3D11InputLayout> m_inputLayout;
        ComPtr<ID3D11Buffer> m_vertexBuffer;
        ComPtr<ID3D11Buffer> m_indexBuffer;

        ComPtr<ID3D11DepthStencilView> m_depthStencilView;
        ComPtr<ID3D11Buffer> m_transformBuffer;

        float m_aspectRatio = 1.0f;
    };
}
