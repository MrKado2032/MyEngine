#pragma once

extern Application* create_application();

int main()
{
	Application* app = create_application();
	app->run();
	delete app;

	return 0;
}