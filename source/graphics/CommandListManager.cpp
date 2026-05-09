#include "pch.h"
#include "CommandListManager.h"

bool CommandQueue::initialize(ID3D12Device* pDevice, const CommandQueueCreateInfo& create_info)
{
	D3D12_COMMAND_QUEUE_DESC cmd_queue_desc{};
	cmd_queue_desc.Type = create_info.type;
	cmd_queue_desc.Flags = create_info.flags;
	cmd_queue_desc.Priority = create_info.priority;
	cmd_queue_desc.NodeMask = 0;

	if (FAILED(pDevice->CreateCommandQueue(&cmd_queue_desc, IID_PPV_ARGS(m_command_queue.GetAddressOf()))))
	{
		spdlog::error("failed to create command queue");
		return false;
	}

	if (FAILED(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf()))))
	{
		spdlog::error("failed to create fence");
		return false;
	}

	m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_fence_event)
	{
		spdlog::error("failed to create fence event");
		return false;
	}

	return true;
}

void CommandQueue::destroy()
{
	if (m_fence_event)
	{
		CloseHandle(m_fence_event);
		m_fence_event = nullptr;
	}

	m_fence.Reset();
	m_command_queue.Reset();

	m_next_fence_value = 1;
}

bool CommandQueue::is_valid() const
{
	return m_command_queue != nullptr;
}

uint64_t CommandQueue::signal()
{
	auto const fence_value = m_next_fence_value++;
	m_command_queue->Signal(m_fence.Get(), fence_value);
	m_next_fence_value++;

	return fence_value;
}

void CommandQueue::wait_for_fence(uint64_t fence_value)
{
	if (m_fence->GetCompletedValue() < fence_value)
	{
		m_fence->SetEventOnCompletion(fence_value, m_fence_event);
		WaitForSingleObject(m_fence_event, INFINITE);
	}
}

void CommandQueue::wait_idle()
{
	auto const fence_value = signal();
	wait_for_fence(fence_value);
}

void CommandQueue::execute_command(ID3D12CommandList* cmd_list)
{
	ID3D12CommandList* cmd_lists[] = { cmd_list };
	m_command_queue->ExecuteCommandLists(_countof(cmd_lists), cmd_lists);
}

uint64_t CommandQueue::execute_and_signal(ID3D12CommandList* cmd_list)
{
	execute_command(cmd_list);
	return signal();
}

bool CommandListManager::initialize(ID3D12Device* pDevice)
{
	if (!pDevice)
	{
		spdlog::error("failed to initialize command list manager: device is nullptr");
		return false;
	}

	CommandQueueCreateInfo graphics_queue_ci{};
	graphics_queue_ci.type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (!m_cmd_queues[static_cast<size_t>(CommandQueueType::Graphics)].initialize(pDevice, graphics_queue_ci))
	{
		spdlog::error("failed to create graphics queue");
		return false;
	}

	return true;
}

void CommandListManager::destroy()
{
	idle_gpu();

	for (auto& queue : m_cmd_queues)
	{
		if (queue.is_valid())
		{
			queue.destroy();
		}
	}
}

bool CommandListManager::create_command_list(ID3D12Device6* pDevice, ComPtr<ID3D12GraphicsCommandList6>& cmd_list, D3D12_COMMAND_LIST_TYPE type)
{
	if (!pDevice)
	{
		spdlog::error("failed to create command list: device is nullptr");
		return false;
	}

	if (FAILED(pDevice->CreateCommandList1(0, type, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(cmd_list.GetAddressOf()))))
	{
		spdlog::error("failed to create command list");
		return false;
	}

	return true;
}

bool CommandListManager::create_command_allocator(ID3D12Device6* pDevice, ComPtr<ID3D12CommandAllocator>& cmd_allocator, D3D12_COMMAND_LIST_TYPE type)
{
	if (!pDevice)
	{
		spdlog::error("failed to create command allocator: device is nullptr");
		return false;
	}

	if (FAILED(pDevice->CreateCommandAllocator(type, IID_PPV_ARGS(cmd_allocator.GetAddressOf()))))
	{
		spdlog::error("failed to create command allocator");
		return false;
	}

	return true;
}

void CommandListManager::idle_gpu()
{
	for (auto& queue : m_cmd_queues)
	{
		if (queue.is_valid())
		{
			queue.wait_idle();
		}
	}
}

CommandQueue& CommandListManager::get_queue(CommandQueueType type)
{
	const auto index = static_cast<size_t>(type);

	if (index >= m_cmd_queues.size())
	{
		spdlog::error("invalid command queue type");
		return m_cmd_queues[static_cast<size_t>(CommandQueueType::Graphics)];
	}

	return m_cmd_queues[index];
}
