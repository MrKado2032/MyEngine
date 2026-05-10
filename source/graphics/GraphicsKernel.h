#pragma once

struct ID3D12Device6;
struct IDXGIFactory7;
class DescriptorAllocator;
class CommandListManager;
class RootSignatureManager;
class PipelineStateManager;

namespace GraphicsKernel
{
	inline constexpr uint32_t kFrameCount = 2;

	bool initialize();
	void destroy();

	ID3D12Device6* get_device();
	IDXGIFactory7* get_dxgi_factory();

	DescriptorAllocator& get_rtv_allocator();
	CommandListManager& get_cmd_manager();

	RootSignatureManager& get_root_signature_manager();
	PipelineStateManager& get_pso_manager();
}