module;

#include <cstdint>
#include <DirectXMath.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <stdexcept>

module RenderEngine.Rendering;

import :Mesh;
import :Vertex;

namespace RenderEngine
{
	void Mesh::InitPyramid(ID3D11Device& device)
	{
		Vertex vertices[] =
		{
			// Side 1 (-X)
			{ -1, -1, -1, 1, 0, 0, 0, 0, 0, 0, 0 }, // 0
			{ -1, -1,  1, 0, 1, 0, 0, 0, 0, 1, 0 }, // 1
			{ 0,  1,  0, 0, 1, 1, 0, 0, 0, 0.5f, 1 }, // 2

			// Side 2 (+Z)
			{ -1, -1,  1, 0, 1, 0, 0, 0, 0, 0, 0 }, // 3
			{ 1, -1,  1, 0, 0, 1, 0, 0, 0, 1, 0 }, // 4
			{ 0,  1,  0, 0, 1, 1, 0, 0, 0, 0.5f, 1 }, // 5

			// Side 3 (+X)
			{ 1, -1,  1, 0, 0, 1, 0, 0, 0, 0, 0 }, // 6
			{ 1, -1, -1, 1, 1, 0, 0, 0, 0, 1, 0 }, // 7
			{ 0,  1,  0, 0, 1, 1, 0, 0, 0, 0.5f, 1 }, // 8

			// Side 4 (-Z)
			{ 1, -1, -1, 1, 1, 0, 0, 0, 0, 0, 0 }, // 9
			{ -1, -1, -1, 1, 0, 0, 0, 0, 0, 1, 0 }, // 10
			{ 0,  1,  0, 0, 1, 1, 0, 0, 0, 0.5f, 1 }, // 11

			// Bottom (-Y)
			{ -1, -1, -1, 1, 0, 0, 0, 0, 0, 0, 0 }, // 12
			{ -1, -1,  1, 0, 1, 0, 0, 0, 0, 0, 1 }, // 13
			{ 1, -1,  1, 0, 0, 1, 0, 0, 0, 1, 1 }, // 14
			{ 1, -1, -1, 1, 1, 0, 0, 0, 0, 1, 0 }  // 15
		};
		const UINT numOfVertices = sizeof(vertices) / sizeof(Vertex);

		constexpr std::uint16_t indices[] =
		{
			// Sides
			0, 1, 2,
			3, 4, 5,
			6, 7, 8,
			9, 10, 11,

			// Bottom
			12, 14, 13,
			12, 15, 14
		};
		const UINT numOfIndices = sizeof(indices) / sizeof(std::uint16_t);

		InitNormals(vertices, indices, numOfIndices);
		InitBuffers(device, vertices, numOfVertices, indices, numOfIndices);
	}

	void Mesh::InitCube(ID3D11Device & device)
	{
		Vertex vertices[] =
		{
			// Front (-Z)
			{ -1, -1, -1, 1, 0, 0, 0, 0, 0, 0, 1 }, // 0
			{ -1,  1, -1, 0, 1, 0, 0, 0, 0, 0, 0 }, // 1
			{ 1,  1, -1, 0, 0, 1, 0, 0, 0, 1, 0 }, // 2
			{ 1, -1, -1, 1, 1, 0, 0, 0, 0, 1, 1 }, // 3

			// Back (+Z)
			{ -1, -1,  1, 0, 1, 1, 0, 0, 0, 0, 1 }, // 4
			{ -1,  1,  1, 1, 0, 1, 0, 0, 0, 0, 0 }, // 5
			{ 1,  1,  1, 1, 1, 1, 0, 0, 0, 1, 0 }, // 6
			{ 1, -1,  1, 1, 0.5f, 0, 0, 0, 0, 1, 1 }, // 7

			// Left (-X)
			{ -1, -1, -1, 1, 0, 0, 0, 0, 0, 0, 1 }, // 8
			{ -1, -1,  1, 0, 1, 1, 0, 0, 0, 0, 0 }, // 9
			{ -1,  1,  1, 1, 0, 1, 0, 0, 0, 1, 0 }, // 10
			{ -1,  1, -1, 0, 1, 0, 0, 0, 0, 1, 1 }, // 11

			// Right (+X)
			{ 1, -1, -1, 1, 1, 0, 0, 0, 0, 0, 1 }, // 12
			{ 1,  1, -1, 0, 0, 1, 0, 0, 0, 0, 0 }, // 13
			{ 1,  1,  1, 1, 1, 1, 0, 0, 0, 1, 0 }, // 14
			{ 1, -1,  1, 1, 0.5f, 0, 0, 0, 0, 1, 1 }, // 15

			// Top (+Y)
			{ -1,  1, -1, 0, 1, 0, 0, 0, 0, 0, 1 }, // 16
			{ -1,  1,  1, 1, 0, 1, 0, 0, 0, 0, 0 }, // 17
			{ 1,  1,  1, 1, 1, 1, 0, 0, 0, 1, 0 }, // 18
			{ 1,  1, -1, 0, 0, 1, 0, 0, 0, 1, 1 }, // 19

			// Bottom (-Y)
			{ -1, -1, -1, 1, 0, 0, 0, 0, 0, 0, 1 }, // 20
			{ 1, -1, -1, 1, 1, 0, 0, 0, 0, 0, 0 }, // 21
			{ 1, -1,  1, 1, 0.5f, 0, 0, 0, 0, 1, 0 }, // 22
			{ -1, -1,  1, 0, 1, 1, 0, 0, 0, 1, 1 }  // 23
		};
		const UINT numOfVertices = sizeof(vertices) / sizeof(Vertex);

		// Each group of three indices defines one triangle.
		// The winding order matches the default D3D11 rasterizer.
		constexpr std::uint16_t indices[] =
		{
			// Front
			0, 1, 2,
			0, 2, 3,

			// Back
			4, 6, 5,
			4, 7, 6,

			// Left
			8, 9, 10,
			8, 10, 11,

			// Right
			12, 13, 14,
			12, 14, 15,

			// Top
			16, 17, 18,
			16, 18, 19,

			// Bottom
			20, 21, 22,
			20, 22, 23
		};
		const UINT numOfIndices = sizeof(indices) / sizeof(std::uint16_t);

		InitNormals(vertices, indices, numOfIndices);
		InitBuffers(device, vertices, numOfVertices, indices, numOfIndices);
	}

	void Mesh::InitNormals(Vertex* const vertices, const std::uint16_t* const indices, const UINT numOfIndices)
	{
		assert(numOfIndices % 3 == 0);

		for (UINT i = 0; i < numOfIndices; i += 3)
		{
			Vertex& v0 = vertices[indices[i]];
			Vertex& v1 = vertices[indices[i+1]];
			Vertex& v2 = vertices[indices[i+2]];
			DirectX::XMVECTOR a = { v0.x, v0.y, v0.z };
			DirectX::XMVECTOR b = { v1.x, v1.y, v1.z };
			DirectX::XMVECTOR c = { v2.x, v2.y, v2.z };
			DirectX::XMVECTOR u = DirectX::XMVectorSubtract(b, a);
			DirectX::XMVECTOR v = DirectX::XMVectorSubtract(c, a);
			DirectX::XMVECTOR normal = DirectX::XMVector3Cross(u, v);
			normal = DirectX::XMVector3Normalize(normal);
			DirectX::XMFLOAT3 normalFloat;
			DirectX::XMStoreFloat3(&normalFloat, normal);
			v0.nx = v1.nx = v2.nx = normalFloat.x;
			v0.ny = v1.ny = v2.ny = normalFloat.y;
			v0.nz = v1.nz = v2.nz = normalFloat.z;
		}
	}

	void Mesh::InitBuffers(ID3D11Device& device, const Vertex* const vertices, const UINT numOfVertices, const std::uint16_t* const indices, const UINT numOfIndices)
	{
		m_indexCount = numOfIndices;

		// Create the vertex buffer.
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * numOfVertices);
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initialData{};
		initialData.pSysMem = vertices;

		HRESULT result = device.CreateBuffer(
			&bufferDesc,
			&initialData,
			m_vertexBuffer.GetAddressOf()
		);
		CheckHR(result, "CreateBuffer failed");


		// Describe the index buffer.
		D3D11_BUFFER_DESC indexDesc{};
		indexDesc.ByteWidth = static_cast<UINT>(sizeof(std::uint16_t) * numOfIndices);
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		// Provide the initial index data.
		D3D11_SUBRESOURCE_DATA indexData{};
		indexData.pSysMem = indices;

		// Create the GPU index buffer.
		result = device.CreateBuffer(
			&indexDesc,
			&indexData,
			m_indexBuffer.GetAddressOf()
		);
		CheckHR(result, "Create index buffer failed");
	}

	void Mesh::CheckHR(HRESULT result, const char* message)
	{
		if (FAILED(result))
			throw std::runtime_error(message);
	}
}