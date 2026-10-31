module;

#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>

export module RenderEngine.Rendering:Mesh;

import :Vertex;

using Microsoft::WRL::ComPtr;

export namespace RenderEngine
{
	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh() = default;

		void InitPyramid(ID3D11Device& device);
		void InitCube(ID3D11Device& device);

		[[nodiscard]] ID3D11Buffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }
		[[nodiscard]] ID3D11Buffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }
		[[nodiscard]] UINT GetIndexCount() const { return m_indexCount; }

	private:
		void InitNormals(Vertex* vertices, const std::uint16_t* indices, UINT numOfIndices);
		void InitBuffers(ID3D11Device& device, const Vertex* vertices, UINT numOfVertices, const std::uint16_t* indices, UINT numOfIndices);

		static void CheckHR(HRESULT result, const char* message);

		ComPtr<ID3D11Buffer> m_vertexBuffer;
		ComPtr<ID3D11Buffer> m_indexBuffer;
		UINT m_indexCount = 0;
	};
}