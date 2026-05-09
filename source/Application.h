#pragma once
#include <memory>
#include <cstdint>
#include <string>

class Window;
class Application
{
public:
	Application();
	virtual ~Application();

	void run();

protected:
	virtual void start() = 0;
	virtual void update(float dt) = 0;

private:
	std::unique_ptr<Window> m_window;
};

Application* create_application();