#pragma once
#include <Dark/Application.h>

class Application : public Dark::Application
{
public:
	Application() = default;
	~Application() override = default;

	void onInit() override;
	void onDestroy() override;
	void onUpdate(float dT) override;

private:

};