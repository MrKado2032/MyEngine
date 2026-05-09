#pragma once
#include "DescriptorHeap.h"

struct RenderTargetViewDetails
{
	uint32_t init_width = UINT32_MAX;
	uint32_t init_height = UINT32_MAX;
	HWND window_handle = nullptr;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
};

class SwapChain
{
public:
	struct FrameResource
	{
		ComPtr<ID3D12Resource> buffer;
		DescriptorHandle handle;
	};

	SwapChain() = default;
	~SwapChain() = default;

	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;

	bool initialize(const RenderTargetViewDetails& details);
	void destroy();

	void present();

	uint32_t get_current_back_buffer_index() const;
	FrameResource& get_current_frame_resource();

private:
	bool create_swap_chain(const RenderTargetViewDetails& details);
	bool create_render_target_view();

	ComPtr<IDXGISwapChain4> m_swap_chain;
	std::vector<FrameResource> m_frame_resources;
};