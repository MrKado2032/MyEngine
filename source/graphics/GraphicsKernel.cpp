#include "pch.h"
#include "GraphicsKernel.h"

#include "DescriptorHeap.h"
#include "CommandListManager.h"

#include "RootSignature.h"
#include "PipelineState.h"

namespace GraphicsKernel
{
	// --- Static Objects --- //

	static ComPtr<ID3D12Device6> s_dx12_device;
	static ComPtr<IDXGIFactory7> s_dxgi_factory;

	static DescriptorHeap s_rtv_heap{};
	static DescriptorAllocator s_rtv_allocator{};

	static CommandListManager s_command_list_manager{};

	static RootSignatureManager s_root_signature_manager{};
	static PipelineStateManager s_pipeline_state_manager{};

	// ---------------------- //

	// --- Private functions --- //

	bool setup_debug_layer(UINT& out_dxgi_flags);
	bool select_suitable_adapter(ComPtr<IDXGIAdapter4>& out_adapter);
	bool create_device(const ComPtr<IDXGIAdapter4>& adapter, D3D_FEATURE_LEVEL level);

	// ------------------------ //

	bool initialize()
	{
		UINT dxgi_flags = 0;
#ifdef _DEBUG
		if (!setup_debug_layer(dxgi_flags))
		{
			spdlog::warn("failed to setup debug layer");
		}
#endif

		if (FAILED(CreateDXGIFactory2(dxgi_flags, IID_PPV_ARGS(s_dxgi_factory.GetAddressOf()))))
		{
			spdlog::error("failed to create dxgi factory");
			return false;
		}

		// アダプターの選定
		ComPtr<IDXGIAdapter4> adapter;
		if (!select_suitable_adapter(adapter))
		{
			spdlog::error("failed to select suitable adapter");
			return false;
		}

		// デバイスの作成
		if (!create_device(adapter, D3D_FEATURE_LEVEL_12_0))
		{
			spdlog::error("failed to create device");
			return false;
		}

		// RTVヒープの作成
		if (!s_rtv_heap.initialize(s_dx12_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kFrameCount, false))
		{
			spdlog::error("failed to create rtv heap");
			return false;
		}

		// RTVアロケーターの作成
		if (!s_rtv_allocator.initialize(s_rtv_heap))
		{
			spdlog::error("failed to create rtv allocator");
			return false;
		}

		// コマンドマネージャーの作成
		if (!s_command_list_manager.initialize(s_dx12_device.Get()))
		{
			spdlog::error("failed to initialize command list manager");
			return false;
		}

		// ルートシグネチャの作成
		if (!s_root_signature_manager.initialize(s_dx12_device.Get()))
		{
			spdlog::error("failed to initialize root signature");
			return false;
		}

		// PSOの作成
		if (!s_pipeline_state_manager.initialize(s_dx12_device.Get(), s_root_signature_manager))
		{
			spdlog::error("failed to initialize pso manager");
			return false;
		}

		return true;
	}

	void destroy()
	{
		s_command_list_manager.destroy();

		s_pipeline_state_manager.destroy();
		s_root_signature_manager.destroy();

		s_rtv_allocator.destroy();

		s_rtv_heap.destroy();

		s_dx12_device.Reset();
		s_dxgi_factory.Reset();
	}

	bool setup_debug_layer(UINT& out_dxgi_flags)
	{
		ComPtr<ID3D12Debug6> debug_controller;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debug_controller.GetAddressOf()))))
		{
			debug_controller->EnableDebugLayer();
			out_dxgi_flags |= DXGI_CREATE_FACTORY_DEBUG;

			return true;
		}

		return false;
	}

	bool select_suitable_adapter(ComPtr<IDXGIAdapter4>& out_adapter)
	{
		for (UINT i = 0; s_dxgi_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(out_adapter.ReleaseAndGetAddressOf())) != DXGI_ERROR_NOT_FOUND; i++)
		{
			DXGI_ADAPTER_DESC3 adp_desc{};
			if (FAILED(out_adapter->GetDesc3(&adp_desc)))
			{
				spdlog::error("failed to get adapter desc 3");
				return false;
			}

			if (adp_desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) continue;

			if (SUCCEEDED(D3D12CreateDevice(out_adapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)))
			{
				return true;
			}
		}

		return false;
	}

	bool create_device(const ComPtr<IDXGIAdapter4>& adapter, D3D_FEATURE_LEVEL level)
	{
		// デバイスの作成
		auto hr = D3D12CreateDevice(adapter.Get(), level, IID_PPV_ARGS(s_dx12_device.GetAddressOf()));
		
		if (FAILED(hr))
		{
			spdlog::error("failed to create dx12 device, HRESULT = 0x{:08X}", static_cast<unsigned int>(hr));
			return false;
		}

		return true;
	}

	ID3D12Device6* get_device()
	{
		return s_dx12_device.Get();
	}

	IDXGIFactory7* get_dxgi_factory()
	{
		return s_dxgi_factory.Get();
	}

	DescriptorAllocator& get_rtv_allocator()
	{
		return s_rtv_allocator;
	}

	CommandListManager& get_cmd_manager()
	{
		return s_command_list_manager;
	}

	RootSignatureManager& get_root_signature_manager()
	{
		return s_root_signature_manager;
	}

	PipelineStateManager& get_pso_manager()
	{
		return s_pipeline_state_manager;
	}
}
