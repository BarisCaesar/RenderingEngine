#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "CameraContainer.h"
#include "PointLight.h"
#include "TestCube.h"
#include "Model.h"
#include "ScriptCommander.h"
#include "BlurOutlineRenderGraph.h"
#include <string>
#include "RMath.h"


class App
{
public:
	App(const std::string& commandLine = "");
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame(float dt);
	void HandleInput(float dt);
	void ShowImguiDemoWindow();
private:
	std::string commandLine;
	bool showDemoWindow = false;
	ImguiManager imgui;
	Window wnd;
	ScriptCommander scriptCommander;
	RenderGraph::BlurOutlineRenderGraph rg{ wnd.Gfx() };
	Timer timer;
	float speedFactor = 1.f;
	CameraContainer cameras;
	PointLight light;
	TestCube cube = { wnd.Gfx(), 4.f };
	TestCube cube2 = { wnd.Gfx(), 4.f };
	Model sponza{ wnd.Gfx(), "Models\\sponza\\sponza.obj", 1.f / 20.f };
	Model goblin{ wnd.Gfx(), "Models\\goblin\\GoblinX.obj", 4.f };
	Model nano{ wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj", 2.f };

};

