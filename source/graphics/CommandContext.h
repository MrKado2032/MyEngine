#pragma once

class CommandContext
{
public:
	CommandContext() = default;
	~CommandContext() = default;

	CommandContext(const CommandContext&) = delete;
	CommandContext& operator=(const CommandContext&) = delete;

	// コマンドコンテキストの初期化処理
	bool initialize(ID3D12Device6* pDevice, D3D12_COMMAND_LIST_TYPE type);

	// コンテキストにコマンドを積める状態にする
	void begin();

	// コンテキストにコマンドを実行可能状態にする
	void end();

	// リソースバリアの遷移処理
	void transition_barrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before_state, D3D12_RESOURCE_STATES after_state);

	// ビューポートのセット
	void set_viewport(const uint32_t width, const uint32_t height, const float min_depth = 1.0, const float max_depth = 1.0);

	// シザー矩形のセット
	void set_scissor(const uint32_t width, const uint32_t height);

	// ルートシグネチャのセット
	void set_root_signature(ID3D12RootSignature* pRootSignature);

	// PSOのセット
	void set_pso(ID3D12PipelineState* pso);

	// PrimitiveTopologyのセット
	void set_primitive_topolog(D3D12_PRIMITIVE_TOPOLOGY topology);
	
	// 頂点バッファービューのセット
	void set_vertex_buffer_view(D3D12_VERTEX_BUFFER_VIEW& view);

	// インデックスバッファービューのセット
	void set_index_buffer_view(D3D12_INDEX_BUFFER_VIEW& view);

	// インデックスドロー
	void draw_indexed(uint32_t index_count, uint32_t instance_count);

	// RTVのセット
	void set_render_target(D3D12_CPU_DESCRIPTOR_HANDLE handle);

	// RTVのクリア処理
	void clear_render_target(D3D12_CPU_DESCRIPTOR_HANDLE handle, const FLOAT clear_color[4]);

	ID3D12GraphicsCommandList6* get_cmd_list() const
	{
		return m_cmd_list.Get();
	}

private:
	ComPtr<ID3D12GraphicsCommandList6> m_cmd_list;
	ComPtr<ID3D12CommandAllocator> m_cmd_allocator;
};