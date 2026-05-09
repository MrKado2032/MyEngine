#include "pch.h"
#include "RenderTargetView.h"

#include "GraphicsKernel.h"

bool RenderTargetView::initialize(const RenderTargetViewDetails& details)
{
	auto* device = GraphicsKernel::get_device();

	// RTVヒープの作成
	m_rtv_heap = std::make_unique<DescriptorHeap>();
	if (!m_rtv_heap->initialize(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false))
	{
		spdlog::error("failed to create rtv heap");
		return false;
	}

	// RTVアロケーターの作成
	m_rtv_allocator = std::make_unique<DescriptorAllocator>();
	if (!m_rtv_allocator->initialize(*m_rtv_heap))
	{
		spdlog::error("failed to create rtv allocator");
		return false;
	}

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

void RenderTargetView::destroy()
{
	m_rtv_allocator->destroy();
	m_rtv_allocator.reset();

	m_rtv_heap->destroy();
	m_rtv_heap.reset();
}

bool RenderTargetView::create_swap_chain(const RenderTargetViewDetails& details)
{
	// スワップチェインの作成
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc{};
	swap_chain_desc.Width = static_cast<UINT>(details.init_width);
	swap_chain_desc.Height = static_cast<UINT>(details.init_height);
	swap_chain_desc.BufferCount = static_cast<UINT>(details.buffer_count);
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swap_chain_desc.Flags = 0;
	swap_chain_desc.Format = details.format;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
	swap_chain_desc.Stereo = FALSE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	ComPtr<IDXGISwapChain1> swapchain_1;
	if (FAILED(GraphicsKernel::get_dxgi_factory()->CreateSwapChainForHwnd(nullptr, details.window_handle, &swap_chain_desc, nullptr, nullptr, swapchain_1.GetAddressOf())))
	{
		spdlog::error("failed to create swap chain 1");
		return false;
	}

	if (FAILED(swapchain_1.As(&m_swap_chain)))
	{
		spdlog::error("failed to convert swap chain 4");
		return false;
	}

	return true;
}

bool RenderTargetView::create_render_target_view()
{
	// RTVの作成
	m_frame_resources.resize(GraphicsKernel::kFrameCount);
	for (UINT i = 0; i < GraphicsKernel::kFrameCount; i++)
	{
		m_swap_chain->GetBuffer(i, IID_PPV_ARGS(m_frame_resources[i].buffer.GetAddressOf()));

		m_frame_resources[i].handle = m_rtv_allocator->allocate();
		GraphicsKernel::get_device()->CreateRenderTargetView(m_frame_resources[i].buffer.Get(), nullptr, m_frame_resources[i].handle.get_cpu_handle());
	}

	return true;
}
