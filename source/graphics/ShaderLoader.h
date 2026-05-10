#pragma once

struct Shader
{
	std::vector<char> vs;
	std::vector<char> ps;

	bool is_valid() const
	{
		return !vs.empty() && !ps.empty();
	}

	D3D12_SHADER_BYTECODE get_vs_shader_code() const
	{
		return { vs.data(), static_cast<UINT>(vs.size()) };
	}

	D3D12_SHADER_BYTECODE get_ps_shader_code() const
	{
		return { ps.data(), static_cast<UINT>(ps.size()) };
	}
};

class ShaderLoader
{
public:
	ShaderLoader() = default;
	~ShaderLoader() = default;

	ShaderLoader(const ShaderLoader&) = delete;
	ShaderLoader& operator=(const ShaderLoader&) = delete;

	ShaderLoader(ShaderLoader&&) = delete;
	ShaderLoader& operator=(ShaderLoader&&) = delete;

	static Shader load_from_path(const std::wstring& vs_path, const std::wstring& ps_path);

private:
	static std::vector<char> load_code_from_path(const std::wstring& path);
	
};