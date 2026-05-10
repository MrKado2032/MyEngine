#pragma once
#include <memory>
#include <cstdint>
#include <string>
#include "graphics/Renderer.h"


class Renderer;
class Window;
class Application
{
public:
	Application();
	virtual ~Application();

	// アプリケーションの実行
	void run();

protected:
	// ゲーム開始時に実行されます
	virtual void start() = 0;

	// 毎フレーム実行されます
	virtual void update(float dt) = 0;

private:
	std::unique_ptr<Window> m_window;
	std::unique_ptr<Renderer> m_renderer;
};

Application* create_application();