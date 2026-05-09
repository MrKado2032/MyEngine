#pragma once
#include <Windows.h>

struct GLFWwindow;
class Window
{
public:
	Window() = default;
	~Window() = default;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	bool create(const uint32_t width, const uint32_t height, const std::string& title);
	void destroy();

	bool should_close() const;
	void update_events();

	GLFWwindow* get_window() const
	{
		return m_window;
	}

	HWND get_window_handle() const;

private:
	GLFWwindow* m_window = nullptr;

};