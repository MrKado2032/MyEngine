#pragma once

struct ID3D12Device6;
struct IDXGIFactory7;
class Window;
class DescriptorAllocator;
namespace GraphicsKernel
{
	inline constexpr uint32_t kFrameCount = 2;

	bool initialize(const Window& window);
	void destroy();

	ID3D12Device6* get_device();
	IDXGIFactory7* get_dxgi_factory();

	DescriptorAllocator& get_rtv_allocator();
}