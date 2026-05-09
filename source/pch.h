#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <wrl/client.h>

#include <spdlog/spdlog.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <cstdint>
#include <algorithm>

using Microsoft::WRL::ComPtr;
using namespace DirectX;