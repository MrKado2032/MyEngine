#include "pch.h"
#include "PipelineState.h"

#include "RootSignature.h"
#include "ShaderLoader.h"

bool PipelineStateManager::initialize(ID3D12Device6* pDevice, const RootSignatureManager& root_sig_mgr)
{
	if (!pDevice)
	{
		spdlog::error("failed to initialize pipeline state manager: device is nullptr");
		return false;
	}

	if (!create_pso_2d_default(pDevice, root_sig_mgr.get_root_signature(RootSignatureType::Default2D)))
	{
		return false;
	}

	return true;
}

void PipelineStateManager::destroy()
{
	for (auto& pso : m_fixed_pipeline_states)
	{
		pso.Reset();
	}
}

ID3D12PipelineState* PipelineStateManager::get_pso(PipelineStateType type) const
{
	auto const index = static_cast<size_t>(type);
	if (index >= m_fixed_pipeline_states.size())
	{
		spdlog::error("failed to get pipeline state: invalid type");
		return nullptr;
	}

	auto const& pso = m_fixed_pipeline_states[index];
	if (pso)
	{
		return pso.Get();
	}

	spdlog::error("failed to get pipeline state: pipeline state is nullptr");

	return nullptr;
}

bool PipelineStateManager::create_pso_2d_default(ID3D12Device6* pDevice, ID3D12RootSignature* pRootSignature)
{
	D3D12_INPUT_ELEMENT_DESC input_layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	auto const shader = ShaderLoader::load_from_path(L"source/shaders/Basic2D_VertexShader.cso", L"source/shaders/Basic2D_PixelShader.cso");

	GraphicsPipelineStateInfo gpsi_2d{};
	gpsi_2d.pRootSignature = pRootSignature;
	gpsi_2d.inputLayout.NumElements = _countof(input_layout);
	gpsi_2d.inputLayout.pInputElementDescs = input_layout;
	gpsi_2d.vs = shader.get_vs_shader_code();
	gpsi_2d.ps = shader.get_ps_shader_code();

	ComPtr<ID3D12PipelineState> pipeline_state;
	if (!PipelineStateBuilder::build(pDevice, gpsi_2d, pipeline_state))
	{
		spdlog::error("failed to create pso");
		return false;
	}

	m_fixed_pipeline_states[static_cast<size_t>(PipelineStateType::Default2D)] = std::move(pipeline_state);

	return true;
}

bool PipelineStateBuilder::build(ID3D12Device6* pDevice, const GraphicsPipelineStateInfo& info, ComPtr<ID3D12PipelineState>& out_pso)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
	pso_desc.VS = info.vs;
	pso_desc.PS = info.ps;
	pso_desc.pRootSignature = info.pRootSignature;
	pso_desc.InputLayout = info.inputLayout;
	pso_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pso_desc.NodeMask = 0;
	pso_desc.BlendState = info.blendDesc;
	pso_desc.RasterizerState = info.rasterizerDesc;
	pso_desc.DepthStencilState.DepthEnable = info.enableDepth ? TRUE : FALSE;
	pso_desc.DepthStencilState.StencilEnable = FALSE;
	pso_desc.NumRenderTargets = 1;
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.SampleDesc.Count = 1;
	pso_desc.SampleDesc.Quality = 0;
	pso_desc.SampleMask = UINT_MAX;
	pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	auto hr = pDevice->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(out_pso.GetAddressOf()));
	if (FAILED(hr))
	{
		spdlog::error("failed to create pso: HRESULT = 0x{:08X}", static_cast<unsigned int>(hr));
		return false;
	}

	return true;
}
