#include "pch.h"
#include "RootSignature.h"

bool RootSignatureManager::initialize(ID3D12Device6* pDevice)
{
	if (!pDevice)
	{
		spdlog::error("failed to initialize root signature: device is nullptr");
		return false;
	}

	if (!create_root_signature_2d_default(pDevice))
	{
		return false;
	}

	return true;
}

void RootSignatureManager::destroy()
{
	for (auto& root_signature : m_fixed_root_signatures)
	{
		root_signature.Reset();
	}
}

ID3D12RootSignature* RootSignatureManager::get_root_signature(RootSignatureType type) const
{
	auto const index = static_cast<size_t>(type);

	if (index >= m_fixed_root_signatures.size())
	{
		spdlog::error("failed to get root signature: invalid type");
		return nullptr;
	}

	auto const& it = m_fixed_root_signatures[index];
	if (it != nullptr)
	{
		return it.Get();
	}

	spdlog::error("failed to get root signature: root signature is nullptr");

	return nullptr;
}

bool RootSignatureManager::create_root_signature_2d_default(ID3D12Device6* pDevice)
{
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc{};
	root_signature_desc.Init_1_1(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> root_signature_binary, error_binary;
	auto hr = D3DX12SerializeVersionedRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1_1, root_signature_binary.GetAddressOf(), error_binary.GetAddressOf());
	if (FAILED(hr))
	{
		if (error_binary)
		{
			spdlog::error("failed to serialize root signature: {}", static_cast<const char*>(error_binary->GetBufferPointer()));
		}

		spdlog::error("failed to serialize root signature: HRESULT = 0x{:08X}", static_cast<unsigned int>(hr));
		return false;
	}

	ComPtr<ID3D12RootSignature> root_signature;
	hr = pDevice->CreateRootSignature(0, root_signature_binary->GetBufferPointer(), root_signature_binary->GetBufferSize(), IID_PPV_ARGS(root_signature.GetAddressOf()));
	if (FAILED(hr))
	{
		spdlog::error("failed to create root signature: HRESULT = 0x{:08X}", static_cast<unsigned int>(hr));
		return false;
	}

	m_fixed_root_signatures[static_cast<size_t>(RootSignatureType::Default2D)] = std::move(root_signature);

	return true;
}
