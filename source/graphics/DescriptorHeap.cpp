#include "pch.h"
#include "DescriptorHeap.h"

DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle, uint32_t allocated_index)
{
	m_cpu_handle = cpu_handle;
	m_gpu_handle = gpu_handle;

	m_allocated_index = allocated_index;
}

void DescriptorHandle::reset()
{
	m_cpu_handle = {};
	m_gpu_handle = {};

	m_allocated_index = UINT32_MAX;
}

bool DescriptorHeap::initialize(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num_descriptors, bool is_visible_shader)
{
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
	heap_desc.Type = type;
	heap_desc.NumDescriptors = static_cast<UINT>(num_descriptors);
	heap_desc.Flags = is_visible_shader ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heap_desc.NodeMask = 0;

	auto hr = pDevice->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(m_descriptor_heap.GetAddressOf()));
	if (FAILED(hr))
	{
		spdlog::error("failed to create descriptor heap: 0x{:08X}", static_cast<unsigned int>(hr));
		return false;
	}

	m_descriptor_size = static_cast<UINT>(pDevice->GetDescriptorHandleIncrementSize(type));

	m_num_descriptors = num_descriptors;

	m_is_visible_shader = is_visible_shader;

	m_start_cpu_handle = m_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	if (is_visible_shader)
	{
		m_start_gpu_handle = m_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
	}

	return true;
}

void DescriptorHeap::destroy()
{
	m_descriptor_heap.Reset();

	m_descriptor_size = 0;
	m_num_descriptors = 0;
	m_is_visible_shader = false;

	m_start_cpu_handle = {};
	m_start_gpu_handle = {};
}

bool DescriptorAllocator::initialize(DescriptorHeap& descriptor_heap)
{
	m_descriptor_heap = &descriptor_heap;

	auto const num_descriptors = m_descriptor_heap->get_num_descriptors();

	m_freedList.resize(static_cast<size_t>(num_descriptors));
	for (uint32_t i = 0; i < num_descriptors; i++)
	{
		m_freedList[i] = num_descriptors - 1 - i;
	}

	return true;
}

void DescriptorAllocator::destroy()
{
	m_descriptor_heap = nullptr;
	m_freedList.clear();
}

DescriptorHandle DescriptorAllocator::allocate()
{
	if (!m_descriptor_heap)
	{
		spdlog::error("failed to allocate descriptor: descriptor heap is null");
		return {};
	}

	if (m_freedList.empty())
	{
		spdlog::error("failed to allocate descriptor: out of memory");
		return {};
	}

	auto const allocate_index = m_freedList.back();
	m_freedList.pop_back();

	auto cpu_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptor_heap->get_start_cpu_handle(), static_cast<UINT>(allocate_index), m_descriptor_heap->get_descriptor_size());
	if (m_descriptor_heap->is_visible_shader())
	{
		auto gpu_handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptor_heap->get_start_gpu_handle(), static_cast<UINT>(allocate_index), m_descriptor_heap->get_descriptor_size());

		return DescriptorHandle(cpu_handle, gpu_handle, allocate_index);
	}

	return DescriptorHandle(cpu_handle, {}, allocate_index);
}

void DescriptorAllocator::free(DescriptorHandle& handle)
{
	if (!m_descriptor_heap)
	{
		spdlog::error("failed to allocate descriptor: descriptor heap is null");
		return;
	}

	auto const allocated_index = handle.get_allocated_index();

	if (allocated_index >= m_descriptor_heap->get_num_descriptors())
	{
		spdlog::error("invalid descriptor index: {}", allocated_index);
		return;
	}

	if (std::ranges::find(m_freedList, allocated_index) != m_freedList.end())
	{
		return;
	}

	m_freedList.push_back(allocated_index);

	handle.reset();
}
