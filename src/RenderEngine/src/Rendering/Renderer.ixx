module;

#include <DirectXMath.h>
#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

export module RenderEngine.Rendering:Renderer;

export import :Camera;
import :Mesh;
import :Renderable;

using Microsoft::WRL::ComPtr;

export namespace RenderEngine
{
    class Renderer
    {
    public:
        void Initialize(HWND hwnd, UINT width, UINT height);
        void Render(float elapsedSeconds, const Camera& camera);
		void ShutDown();

    private:
        void CreateDevice();
        void CreateSwapChain(HWND hwnd, UINT width, UINT height);
        void CreateRenderTarget(UINT width, UINT height);
        void CreateDepthBuffer(UINT width, UINT height);
        void CreateSampler();
        void CreateCubeResources();
        void RenderObject(const Renderable& renderable, const DirectX::XMMATRIX& viewprojection);

        [[nodiscard]] ComPtr<ID3DBlob> CompileShader(const wchar_t* shaderPath, const char* entry, const char* target) const;

        static void CheckHR(HRESULT result, const char* message);

        // Direct3D resources are owned by the renderer.
        ComPtr<ID3D11Device> m_device;
        ComPtr<ID3D11DeviceContext> m_context;
        ComPtr<IDXGISwapChain1> m_swapChain;
        ComPtr<ID3D11RenderTargetView> m_renderTarget;

        ComPtr<ID3D11VertexShader> m_vertexShader;
        ComPtr<ID3D11PixelShader> m_pixelShader;
        ComPtr<ID3D11InputLayout> m_inputLayout;

        ComPtr<ID3D11DepthStencilView> m_depthStencilView;
        ComPtr<ID3D11Buffer> m_transformBuffer;
        ComPtr<ID3D11SamplerState> m_samplerState;

        ComPtr<ID3D11Texture2D> m_checkerTexture;
        ComPtr<ID3D11ShaderResourceView> m_checkerTextureResourceView;

        Mesh m_cubeMesh;
        Mesh m_pyramidMesh;

        Renderable m_cube1;
        Renderable m_cube2;
        Renderable m_cube3;
        Renderable m_pyramid;

        float m_aspectRatio = 1.0f;
    };
}

