#include "pch.h"
#include "Window.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

bool Window::create(const uint32_t width, const uint32_t height, const std::string& title)
{
	if (!glfwInit())
	{
		spdlog::error("failed to initialize glfw");
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
	if (!m_window)
	{
		spdlog::error("failed to create window");
		glfwTerminate();
		return false;
	}

	return true;
}

void Window::destroy()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool Window::should_close() const
{
	return glfwWindowShouldClose(m_window);
}

void Window::update_events()
{
	glfwPollEvents();
}

HWND Window::get_window_handle() const
{
	return glfwGetWin32Window(m_window);
}
