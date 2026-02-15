#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "TestPlane.h"
#include "Mesh.h"
#include <set>


class App
{
public:
	App(const std::string& commandLine = "");
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame();
	void ShowImguiDemoWindow();
private:
	std::string commandLine;
	bool showDemoWindow = false;
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	float speedFactor = 1.f;
	Camera cam;
	PointLight light;
	Model sponza{ wnd.Gfx(), "Models\\sponza\\sponza.obj", 1.f / 20.f };
	//Model goblin{ wnd.Gfx(), "Models\\goblin\\GoblinX.obj", 6.f };
	//Model nano{ wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj", 2.f};
	Model stripey{ wnd.Gfx(),"Models\\stripey\\stripey.obj", 2.f };
	//TestPlane plane{ wnd.Gfx(), 6.f };

};

