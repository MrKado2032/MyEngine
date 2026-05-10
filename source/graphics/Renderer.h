#pragma once
#include "GraphicsKernel.h"
#include "SwapChain.h"
#include "Mesh.h"

class CommandContext;
class Window;
class Renderer
{
public:
	Renderer() = default;
	~Renderer() = default;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	bool initialize(const Window& window);
	void destroy();

	void begin_frame();
	void end_frame();

	void draw_mesh(Mesh& mesh);

private:
	std::array<uint64_t, GraphicsKernel::kFrameCount> m_fence_values;
	SwapChain m_swap_chain{};
	CommandContext* m_current_context = nullptr;
};