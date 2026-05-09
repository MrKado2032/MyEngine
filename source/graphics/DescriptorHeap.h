#pragma once

class DescriptorHandle
{
public:
	DescriptorHandle() = default;
	DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle, uint32_t allocated_index);
	~DescriptorHandle() = default;

	void reset();

	D3D12_CPU_DESCRIPTOR_HANDLE get_cpu_handle() const
	{
		return m_cpu_handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE get_gpu_handle() const
	{
		return m_gpu_handle;
	}

	uint32_t get_allocated_index() const
	{
		return m_allocated_index;
	}

private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_cpu_handle{};
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpu_handle{};
	uint32_t m_allocated_index = UINT32_MAX;
};

class DescriptorHeap
{
public:
	DescriptorHeap() = default;
	~DescriptorHeap() = default;

	DescriptorHeap(const DescriptorHeap&) = delete;
	DescriptorHeap& operator=(const DescriptorHeap&) = delete;

	bool initialize(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num_descriptors, bool is_visible_shader);
	void destroy();

	uint32_t get_descriptor_size() const
	{
		return m_descriptor_size;
	}

	uint32_t get_num_descriptors() const
	{
		return m_num_descriptors;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE get_start_cpu_handle() const
	{
		return m_start_cpu_handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE get_start_gpu_handle() const
	{
		return m_start_gpu_handle;
	}

	ID3D12DescriptorHeap* get_descriptor_heap() const
	{
		return m_descriptor_heap.Get();
	}

	bool is_visible_shader() const
	{
		return m_is_visible_shader;
	}

private:
	ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;
	uint32_t m_descriptor_size = 0;
	uint32_t m_num_descriptors = 0;
	bool m_is_visible_shader = false;

	D3D12_CPU_DESCRIPTOR_HANDLE m_start_cpu_handle{};
	D3D12_GPU_DESCRIPTOR_HANDLE m_start_gpu_handle{};
};

class DescriptorAllocator
{
public:
	DescriptorAllocator() = default;
	~DescriptorAllocator() = default;

	DescriptorAllocator(const DescriptorAllocator&) = delete;
	DescriptorAllocator& operator=(const DescriptorAllocator&) = delete;

	bool initialize(DescriptorHeap& descriptor_heap);
	void destroy();

	DescriptorHandle allocate();
	void free(DescriptorHandle& handle);

private:
	DescriptorHeap* m_descriptor_heap = nullptr;
	std::vector<uint32_t> m_freedList;

};