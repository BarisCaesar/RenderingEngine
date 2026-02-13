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
	App();
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame();
	void ShowImguiDemoWindow();
private:
	bool showDemoWindow = false;
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	float speedFactor = 1.f;
	Camera cam;
	PointLight light;
	Model goblin{ wnd.Gfx(), "Models\\goblin\\GoblinX.obj", 6.f };
	Model nano{ wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj", 2.f};
	Model wall{ wnd.Gfx(),"Models\\brick_wall\\brick_wall.obj", 6.f };
	TestPlane plane{ wnd.Gfx(), 6.f };

};

