#include "pch.h"
#include "Application.h"
#include "platform/Window.h"
#include <iostream>

const uint32_t INIT_WINDOW_WIDTH = 1280;
const uint32_t INIT_WINDOW_HEIGHT = 720;

Application::Application()
{
	// ウインドウの作成
	m_window = std::make_unique<Window>();
	if (!m_window->create(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, "MyEngine"))
	{
		throw std::runtime_error("failed to create window");
	}

	// レンダラーの初期化
	m_renderer = std::make_unique<Renderer>();
	if (!m_renderer->initialize(*m_window))
	{
		throw std::runtime_error("failed to create renderer");
	}
}

Application::~Application()
{
	// レンダラー破棄
	if (m_renderer)
	{
		m_renderer->destroy();
		m_renderer.reset();
	}

	// ウインドウの破棄
	if (m_window)
	{
		m_window->destroy();
		m_window.reset();
	}
}

void Application::run()
{
	// ゲーム開始時実行
	start();
	while (!m_window->should_close())
	{
		// ウインドウに渡されるイベント処理の更新
		m_window->update_events();

		// 初期フレーム処理
		m_renderer->begin_frame();
		
		// 毎フレーム処理
		update(1);

		// 最期フレーム処理
		m_renderer->end_frame();
	}
}
