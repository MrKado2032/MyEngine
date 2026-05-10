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

	// ウインドウの作成をします
	bool create(const uint32_t width, const uint32_t height, const std::string& title);

	// ウインドウの破棄処理をします
	void destroy();

	// ウインドウを閉じるかの判定処理をします
	bool should_close() const;

	// ウインドウイベント処理の更新を行います
	void update_events();

	// ウインドウの実体を渡します
	GLFWwindow* get_window() const
	{
		return m_window;
	}

	// ウインドウハンドル (Windows)を渡します
	HWND get_window_handle() const;

	// ウインドウ横幅を渡します
	uint32_t get_width() const;

	// ウインドウ縦幅を渡します
	uint32_t get_height() const;

private:
	GLFWwindow* m_window = nullptr;

};