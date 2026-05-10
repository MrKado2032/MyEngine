#pragma once

enum class PipelineStateType
{
	Default2D,
	Count
};

class RootSignatureManager;
class PipelineStateManager
{
public:
	PipelineStateManager() = default;
	~PipelineStateManager() = default;

	PipelineStateManager(const PipelineStateManager&) = delete;
	PipelineStateManager& operator=(const PipelineStateManager&) = delete;

	PipelineStateManager(PipelineStateManager&&) = delete;
	PipelineStateManager& operator=(PipelineStateManager&&) = delete;

	bool initialize(ID3D12Device6* pDevice, const RootSignatureManager& root_sig_mgr);
	void destroy();

	ID3D12PipelineState* get_pso(PipelineStateType type) const;

private:
	bool create_pso_2d_default(ID3D12Device6* pDevice, ID3D12RootSignature* pRootSignature);

	std::array<ComPtr<ID3D12PipelineState>, static_cast<size_t>(PipelineStateType::Count)> m_fixed_pipeline_states;

};

struct GraphicsPipelineStateInfo
{
	ID3D12RootSignature* pRootSignature = nullptr;
	D3D12_INPUT_LAYOUT_DESC inputLayout = {};
	D3D12_SHADER_BYTECODE vs;
	D3D12_SHADER_BYTECODE ps;
	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	bool enableDepth = false;
};

class PipelineStateBuilder
{
public:
	static bool build(ID3D12Device6* pDevice, const GraphicsPipelineStateInfo& info, ComPtr<ID3D12PipelineState>& out_pso);
};