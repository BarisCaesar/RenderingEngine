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
	TestPlane bluePlane{ wnd.Gfx(), 6.f, {0.3f, 0.3f, 1.f, 0.f} };
	TestPlane redPlane{ wnd.Gfx(), 6.f, {1.3f, 0.3f, 0.3f, 0.f} };
	//Model goblin{ wnd.Gfx(), "Models\\goblin\\GoblinX.obj", 6.f };
	//Model nano{ wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj", 2.f};
	//Model wall{ wnd.Gfx(),"Models\\brick_wall\\brick_wall.obj", 6.f };
	//TestPlane plane{ wnd.Gfx(), 6.f };

};

