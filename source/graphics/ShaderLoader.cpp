#include "pch.h"
#include "ShaderLoader.h"

#include <fstream>

Shader ShaderLoader::load_from_path(const std::wstring& vs_path, const std::wstring& ps_path)
{
	Shader ret_shader{};

	ret_shader.vs = load_code_from_path(vs_path);
	ret_shader.ps = load_code_from_path(ps_path);

	if (!ret_shader.is_valid())
	{
		spdlog::error("failed to load shader");
		return {};
	}

	return ret_shader;
}

std::vector<char> ShaderLoader::load_code_from_path(const std::wstring& path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		spdlog::error("failed to open file");
		return {};
	}

	const auto file_size = file.tellg();
	if (file_size <= 0)
	{
		spdlog::error("failed to get file size");
		return {};
	}

	std::vector<char> code(static_cast<size_t>(file_size));

	file.seekg(0, std::ios::beg);
	if (!file.read(code.data(), static_cast<std::streamsize>(code.size())))
	{
		spdlog::error("failed to read shader file");
		return {};
	}
	file.close();

	return code;
}
