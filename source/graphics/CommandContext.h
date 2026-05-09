#pragma once

class CommandContext
{
public:
	CommandContext() = default;
	~CommandContext() = default;

	CommandContext(const CommandContext&) = delete;
	CommandContext& operator=(const CommandContext&) = delete;

	bool initialize(ID3D12Device6* pDevice, D3D12_COMMAND_LIST_TYPE type);

	void begin();

	void end();

	void transition_barrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before_state, D3D12_RESOURCE_STATES after_state);
	void set_render_target(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void clear_render_target(D3D12_CPU_DESCRIPTOR_HANDLE handle, const FLOAT clear_color[4]);

	ID3D12GraphicsCommandList6* get_cmd_list() const
	{
		return m_cmd_list.Get();
	}

private:
	ComPtr<ID3D12GraphicsCommandList6> m_cmd_list;
	ComPtr<ID3D12CommandAllocator> m_cmd_allocator;
};