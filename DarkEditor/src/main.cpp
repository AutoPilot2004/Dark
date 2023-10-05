#include "Application.h"

int main()
{
	Application app;
	app.setVSync(false);
	app.setBackgroundColor({ 0.5f, 0.5f, 0.5f, 1.0f });
	app.startWindowMaximized();
	app.run();

	return 0;
}