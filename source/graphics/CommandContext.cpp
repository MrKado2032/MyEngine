#include "pch.h"
#include "CommandContext.h"

bool CommandContext::initialize(ID3D12Device6* pDevice, D3D12_COMMAND_LIST_TYPE type)
{
	// コマンドリストの作成
	if (FAILED(pDevice->CreateCommandList1(0, type, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(m_cmd_list.GetAddressOf()))))
	{
		spdlog::error("failed to create command list");
		return false;
	}

	// コマンドアロケーターの作成
	if (FAILED(pDevice->CreateCommandAllocator(type, IID_PPV_ARGS(m_cmd_allocator.GetAddressOf()))))
	{
		spdlog::error("failed to create command allocator");
		return false;
	}

	return true;
}

void CommandContext::begin()
{
	// コマンドアロケーターの破棄処理
	m_cmd_allocator->Reset();

	// コマンドリストの破棄処理
	m_cmd_list->Reset(m_cmd_allocator.Get(), nullptr);
}

void CommandContext::end()
{
	// コマンドリストをクローズして実行可能状態にする
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

void CommandContext::set_viewport(const uint32_t width, const uint32_t height, const float min_depth, const float max_depth)
{
	CD3DX12_VIEWPORT view_port({ 0, 0, static_cast<FLOAT>(width), static_cast<FLOAT>(height), min_depth, max_depth });
	m_cmd_list->RSSetViewports(1, &view_port);
}

void CommandContext::set_scissor(const uint32_t width, const uint32_t height)
{
	CD3DX12_RECT scissor_rect{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	m_cmd_list->RSSetScissorRects(1, &scissor_rect);
}

void CommandContext::set_root_signature(ID3D12RootSignature * pRootSignature)
{
	m_cmd_list->SetGraphicsRootSignature(pRootSignature);
}

void CommandContext::set_pso(ID3D12PipelineState * pso)
{
	m_cmd_list->SetPipelineState(pso);
}

void CommandContext::set_primitive_topolog(D3D12_PRIMITIVE_TOPOLOGY topology)
{
	m_cmd_list->IASetPrimitiveTopology(topology);
}

void CommandContext::set_vertex_buffer_view(D3D12_VERTEX_BUFFER_VIEW & view)
{
	m_cmd_list->IASetVertexBuffers(0, 1, &view);
}

void CommandContext::set_index_buffer_view(D3D12_INDEX_BUFFER_VIEW & view)
{
	m_cmd_list->IASetIndexBuffer(&view);
}

void CommandContext::draw_indexed(uint32_t index_count, uint32_t instance_count)
{
	m_cmd_list->DrawIndexedInstanced(static_cast<UINT>(index_count), static_cast<UINT>(instance_count), 0, 0, 0);
}

void CommandContext::set_render_target(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	m_cmd_list->OMSetRenderTargets(1, &handle, TRUE, nullptr);
}

void CommandContext::clear_render_target(D3D12_CPU_DESCRIPTOR_HANDLE handle, const FLOAT clear_color[4])
{
	m_cmd_list->ClearRenderTargetView(handle, clear_color, 0, nullptr);
}
