#include "Engine.h"

class GameApp : public Application
{
public:
	GameApp()
	{

	}

	~GameApp()
	{

	}

protected:
	void start() override
	{

	}

	void update(float dt) override
	{

	}
};

Application* create_application()
{
	return new GameApp();
}