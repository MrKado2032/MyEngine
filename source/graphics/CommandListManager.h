#pragma once

struct CommandQueueCreateInfo
{
	D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	D3D12_COMMAND_QUEUE_FLAGS flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	D3D12_COMMAND_QUEUE_PRIORITY priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
};

class CommandQueue
{
public:
	CommandQueue() = default;
	~CommandQueue() = default;

	CommandQueue(const CommandQueue&) = delete;
	CommandQueue& operator=(const CommandQueue&) = delete;

	bool initialize(ID3D12Device* pDevice, const CommandQueueCreateInfo& create_info);
	void destroy();

	bool is_valid() const;

	uint64_t signal();
	void wait_for_fence(uint64_t fence_value);
	void wait_idle();

	void execute_command(ID3D12CommandList* cmd_list);
	uint64_t execute_and_signal(ID3D12CommandList* cmd_list);

	ID3D12CommandQueue* get_command_queue() const
	{
		return m_command_queue.Get();
	}

private:
	ComPtr<ID3D12CommandQueue> m_command_queue;
	ComPtr<ID3D12Fence> m_fence;
	uint64_t m_next_fence_value = 1;
	HANDLE m_fence_event = nullptr;
};

enum class CommandQueueType
{
	Graphics,
	Compute,
	Copy,
	Count
};

class CommandListManager
{
public:
	CommandListManager() = default;
	~CommandListManager() = default;

	CommandListManager(const CommandListManager&) = delete;
	CommandListManager& operator=(const CommandListManager&) = delete;

	bool initialize(ID3D12Device* pDevice);
	void destroy();

	bool create_command_list(ID3D12Device6* pDevice, ComPtr<ID3D12GraphicsCommandList6>& cmd_list, D3D12_COMMAND_LIST_TYPE type);
	bool create_command_allocator(ID3D12Device6* pDevice, ComPtr<ID3D12CommandAllocator>& cmd_allocator, D3D12_COMMAND_LIST_TYPE type);

	void idle_gpu();

	CommandQueue& get_queue(CommandQueueType type);

	ID3D12CommandQueue* get_command_queue(CommandQueueType type)
	{
		return get_queue(type).get_command_queue();
	}

private:
	std::array<CommandQueue, static_cast<size_t>(CommandQueueType::Count)> m_cmd_queues;

};