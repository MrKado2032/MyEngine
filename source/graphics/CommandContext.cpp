#include "pch.h"
#include "CommandContext.h"

bool CommandContext::initialize(ID3D12Device6* pDevice, D3D12_COMMAND_LIST_TYPE type)
{
	if (FAILED(pDevice->CreateCommandList1(0, type, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(m_cmd_list.GetAddressOf()))))
	{
		spdlog::error("failed to create command list");
		return false;
	}

	if (FAILED(pDevice->CreateCommandAllocator(type, IID_PPV_ARGS(m_cmd_allocator.GetAddressOf()))))
	{
		spdlog::error("failed to create command allocator");
		return false;
	}

	return true;
}

void CommandContext::begin()
{
	m_cmd_allocator->Reset();
	m_cmd_list->Reset(m_cmd_allocator.Get(), nullptr);
}

void CommandContext::end()
{
	if (m_cmd_list)
	{
		m_cmd_list->Close();
	}
}

void CommandContext::transition_barrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before_state, D3D12_RESOURCE_STATES after_state)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pResource, before_state, after_state);
	m_cmd_list->ResourceBarrier(1, &barrier);
}

void CommandContext::set_render_target(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	m_cmd_list->OMSetRenderTargets(1, &handle, TRUE, nullptr);
}

void CommandContext::clear_render_target(D3D12_CPU_DESCRIPTOR_HANDLE handle, const FLOAT clear_color[4])
{
	m_cmd_list->ClearRenderTargetView(handle, clear_color, 0, nullptr);
}
