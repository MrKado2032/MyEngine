#include "Application.h"
#include "platform/Window.h"
#include <iostream>

#include "graphics/GraphicsKernel.h"

const uint32_t INIT_WINDOW_WIDTH = 1280;
const uint32_t INIT_WINDOW_HEIGHT = 720;

Application::Application()
{
	m_window = std::make_unique<Window>();

	if (!m_window->create(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, "MyEngine"))
	{
		throw std::runtime_error("failed to create window");
	}

	if (!GraphicsKernel::initialize())
	{
		throw std::runtime_error("failed to initialize graphics kernel");
	}
}

Application::~Application()
{
	GraphicsKernel::destroy();

	if (m_window)
	{
		m_window->destroy();
		m_window.reset();
	}
}

void Application::run()
{
	start();
	while (!m_window->should_close())
	{
		m_window->update_events();
		update(1);
	}
}
