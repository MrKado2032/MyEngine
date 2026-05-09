#pragma once
#include "DescriptorHeap.h"

struct RenderTargetViewDetails
{
	uint32_t init_width = UINT32_MAX;
	uint32_t init_height = UINT32_MAX;
	uint32_t buffer_count = UINT32_MAX;
	HWND window_handle = nullptr;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
};

class RenderTargetView
{
public:
	struct FrameResource
	{
		ComPtr<ID3D12Resource> buffer;
		DescriptorHandle handle;
	};

	RenderTargetView() = default;
	~RenderTargetView() = default;

	RenderTargetView(const RenderTargetView&) = delete;
	RenderTargetView& operator=(const RenderTargetView&) = delete;

	bool initialize(const RenderTargetViewDetails& details);
	void destroy();

private:
	bool create_swap_chain(const RenderTargetViewDetails& details);
	bool create_render_target_view();

	std::unique_ptr<DescriptorHeap> m_rtv_heap;
	std::unique_ptr<DescriptorAllocator> m_rtv_allocator;

	ComPtr<IDXGISwapChain4> m_swap_chain;
	std::vector<FrameResource> m_frame_resources;
};