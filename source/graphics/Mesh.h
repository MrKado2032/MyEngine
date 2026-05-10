#pragma once
#include "VertexInfo.h"

struct Mesh
{
	std::vector<Vertex2D> vertices;
	std::vector<uint16_t> indices;

	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> indexBuffer;

	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};

	uint32_t indexCount = 0;
};

class MeshGenerator
{
public:
	static Mesh generate(const std::vector<Vertex2D>& vertices, const std::vector<uint16_t>& indices);
};