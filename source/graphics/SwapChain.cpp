#include "pch.h"
#include "SwapChain.h"

#include "GraphicsKernel.h"
#include "CommandListManager.h"

bool SwapChain::initialize(const SwapChainDetails& details)
{
	// スワップチェインの作成
	if (!create_swap_chain(details))
	{
		spdlog::error("failed to create swap chain");
		return false;
	}

	// RTVの作成
	if (!create_render_target_view())
	{
		spdlog::error("failed to create rtv");
		return false;
	}

	return true;
}

void SwapChain::destroy()
{
	for (auto& frame : m_frame_resources)
	{
		frame.buffer.Reset();

		GraphicsKernel::get_rtv_allocator().free(frame.handle);
	}
	
	m_swap_chain.Reset();
}

void SwapChain::present()
{
	if (!m_swap_chain)
	{
		spdlog::error("swap chain is null");
		return;
	}

	HRESULT hr = m_swap_chain->Present(1, 0);

	if (FAILED(hr))
	{
		spdlog::error(
			"failed to present. HRESULT = 0x{:08X}",
			static_cast<unsigned int>(hr)
		);
	}
}

uint32_t SwapChain::get_current_back_buffer_index() const
{
	return static_cast<uint32_t>(m_swap_chain->GetCurrentBackBufferIndex());
}

SwapChain::FrameResource& SwapChain::get_current_frame_resource()
{
	return m_frame_resources[get_current_back_buffer_index()];
}

bool SwapChain::create_swap_chain(const SwapChainDetails& details)
{
	// スワップチェインの作成
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc{};
	swap_chain_desc.Width = static_cast<UINT>(details.init_width);
	swap_chain_desc.Height = static_cast<UINT>(details.init_height);
	swap_chain_desc.BufferCount = static_cast<UINT>(GraphicsKernel::kFrameCount);
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swap_chain_desc.Flags = 0;
	swap_chain_desc.Format = details.format;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
	swap_chain_desc.Stereo = FALSE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	const auto cmd_queue = GraphicsKernel::get_cmd_manager().get_command_queue(CommandQueueType::Graphics);

	ComPtr<IDXGISwapChain1> swapchain_1;
	if (FAILED(GraphicsKernel::get_dxgi_factory()->CreateSwapChainForHwnd(cmd_queue, details.window_handle, &swap_chain_desc, nullptr, nullptr, swapchain_1.GetAddressOf())))
	{
		spdlog::error("failed to create swap chain 1");
		return false;
	}

	if (FAILED(swapchain_1.As(&m_swap_chain)))
	{
		spdlog::error("failed to convert swap chain 4");
		return false;
	}

	GraphicsKernel::get_dxgi_factory()->MakeWindowAssociation(details.window_handle, DXGI_MWA_NO_ALT_ENTER);

	return true;
}

bool SwapChain::create_render_target_view()
{
	// RTVの作成
	m_frame_resources.resize(GraphicsKernel::kFrameCount);
	for (UINT i = 0; i < GraphicsKernel::kFrameCount; i++)
	{
		auto hr = m_swap_chain->GetBuffer(i, IID_PPV_ARGS(m_frame_resources[i].buffer.GetAddressOf()));
		if (FAILED(hr))
		{
			spdlog::error(
				"failed to get swap chain buffer. index = {}, HRESULT = 0x{:08X}",
				i,
				static_cast<unsigned int>(hr)
			);
			return false;
		}

		m_frame_resources[i].handle = GraphicsKernel::get_rtv_allocator().allocate();
		GraphicsKernel::get_device()->CreateRenderTargetView(m_frame_resources[i].buffer.Get(), nullptr, m_frame_resources[i].handle.get_cpu_handle());
	}

	return true;
}
