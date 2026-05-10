#include "pch.h"
#include "Mesh.h"

#include "GraphicsKernel.h"

Mesh MeshGenerator::generate(const std::vector<Vertex2D>& vertices, const std::vector<uint16_t>& indices)
{
	Mesh mesh{};
	mesh.vertices = vertices;
	mesh.indices = indices;
	mesh.indexCount = static_cast<uint32_t>(indices.size());

	auto* device = GraphicsKernel::get_device();

	// 頂点バッファーの作成
	auto const vertex_size = vertices.size() * sizeof(Vertex2D);

	auto vertex_heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto vertex_res_desc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<UINT64>(vertex_size));

	auto hr = 
		device->CreateCommittedResource(
			&vertex_heap_props, 
			D3D12_HEAP_FLAG_NONE, 
			&vertex_res_desc, 
			D3D12_RESOURCE_STATE_GENERIC_READ, 
			nullptr, 
			IID_PPV_ARGS(mesh.vertexBuffer.GetAddressOf())
		);

	if (FAILED(hr))
	{
		spdlog::error("failed to create vertex buffer: HRESULT = 0x{:08X}", static_cast<unsigned int>(hr));
		return {};
	}

	Vertex2D* mappedVertex = nullptr;
	mesh.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertex));
	std::memcpy(mappedVertex, vertices.data(), vertex_size);
	mesh.vertexBuffer->Unmap(0, nullptr);

	mesh.vbView.BufferLocation = mesh.vertexBuffer->GetGPUVirtualAddress();
	mesh.vbView.StrideInBytes = sizeof(Vertex2D);
	mesh.vbView.SizeInBytes = static_cast<UINT>(vertex_size);

	// インデックスバッファーの作成
	auto const index_size = indices.size() * sizeof(uint16_t);

	auto index_heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto index_res_desc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<UINT64>(index_size));

	hr =
		device->CreateCommittedResource(
			&index_heap_props,
			D3D12_HEAP_FLAG_NONE,
			&index_res_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mesh.indexBuffer.GetAddressOf())
		);

	if (FAILED(hr))
	{
		spdlog::error("failed to create index buffer: HRESULT = 0x{:08X}", static_cast<unsigned int>(hr));
		return {};
	}

	uint16_t* mappedIndex = nullptr;
	mesh.indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
	std::memcpy(mappedIndex, indices.data(), index_size);
	mesh.indexBuffer->Unmap(0, nullptr);

	mesh.ibView.BufferLocation = mesh.indexBuffer->GetGPUVirtualAddress();
	mesh.ibView.Format = DXGI_FORMAT_R16_UINT;
	mesh.ibView.SizeInBytes = static_cast<UINT>(index_size);

	return mesh;
}
