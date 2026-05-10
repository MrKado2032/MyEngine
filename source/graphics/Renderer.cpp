#include "pch.h"
#include "Renderer.h"

#include "../platform/Window.h"
#include "CommandListManager.h"

bool Renderer::initialize(const Window& window)
{
	if (!GraphicsKernel::initialize())
	{
		spdlog::error("failed to create graphics kernel");
		return false;
	}

	SwapChainDetails swap_chain_details{};
	swap_chain_details.init_width = window.get_width();
	swap_chain_details.init_height = window.get_height();
	swap_chain_details.window_handle = window.get_window_handle();
	swap_chain_details.format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (!m_swap_chain.initialize(swap_chain_details))
	{
		spdlog::error("failed to create swap chain");
		GraphicsKernel::destroy();
		return false;
	}

	for (auto& fence_value : m_fence_values)
	{
		fence_value = 0;
	}

	return true;
}

void Renderer::destroy()
{
	GraphicsKernel::get_cmd_manager().idle_gpu();

	m_swap_chain.destroy();
	GraphicsKernel::destroy();
}

void Renderer::begin_frame()
{
	auto const current_frame_index = m_swap_chain.get_current_back_buffer_index();
	auto const current_frame_fence_value = m_fence_values[current_frame_index];
	SwapChain::FrameResource const& current_back_resource = m_swap_chain.get_current_frame_resource();

	auto& queue = GraphicsKernel::get_cmd_manager().get_queue(CommandQueueType::Graphics);
	queue.wait_for_fence(current_frame_fence_value);

	m_current_context = &GraphicsKernel::get_cmd_manager().begin_context(D3D12_COMMAND_LIST_TYPE_DIRECT, current_frame_index);

	m_current_context->transition_barrier(current_back_resource.buffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_current_context->set_render_target(current_back_resource.handle.get_cpu_handle());

	const FLOAT clear_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	m_current_context->clear_render_target(current_back_resource.handle.get_cpu_handle(), clear_color);
}

void Renderer::end_frame()
{
	auto const current_frame_index = m_swap_chain.get_current_back_buffer_index();
	auto& current_frame_fence_value = m_fence_values[current_frame_index];
	SwapChain::FrameResource const& current_back_resource = m_swap_chain.get_current_frame_resource();

	m_current_context->transition_barrier(current_back_resource.buffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	m_current_context->end();

	current_frame_fence_value = GraphicsKernel::get_cmd_manager().execute_context(*m_current_context, CommandQueueType::Graphics);

	m_swap_chain.present();
}
