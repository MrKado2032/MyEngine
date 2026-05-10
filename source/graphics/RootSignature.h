#pragma once

enum class RootSignatureType
{
	Default2D,
	Count
};

class RootSignatureManager
{
public:
	RootSignatureManager() = default;
	~RootSignatureManager() = default;

	RootSignatureManager(const RootSignatureManager&) = delete;
	RootSignatureManager& operator=(const RootSignatureManager&) = delete;

	RootSignatureManager(RootSignatureManager&&) = delete;
	RootSignatureManager& operator=(RootSignatureManager&&) = delete;

	bool initialize(ID3D12Device6* pDevice);
	void destroy();

	ID3D12RootSignature* get_root_signature(RootSignatureType type) const;

private:
	bool create_root_signature_2d_default(ID3D12Device6* pDevice);

	std::array<ComPtr<ID3D12RootSignature>, static_cast<size_t>(RootSignatureType::Count)> m_fixed_root_signatures;

};