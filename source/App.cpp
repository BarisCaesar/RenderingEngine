#include "App.h"
#include <algorithm>
#include "RMath.h"
#include "imgui/imgui.h"
#include "RUtil.h"
#include "Testing.h"
#include "PerfLog.h"
#include "TestModelProbe.h"
#include "Camera.h"
#include "Channels.h"

namespace dx = DirectX;



App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1280, 720, "Basic App"),
	scriptCommander(TokenizeQuoted(commandLine)),
	light(wnd.Gfx(), {10.f, 5.f, 0.f})
{
	cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "A", dx::XMFLOAT3{ -13.5f,6.0f,3.5f }, 0.0f, PI / 2.0f));
	//cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "B", dx::XMFLOAT3{ -13.5f,28.8f,-6.4f }, PI / 180.0f * 13.0f, PI / 180.0f * 61.0f));
	//cameras.AddCamera(light.ShareCamera());

	cube.SetPos({ 10.0f,5.0f,6.0f });
	/*
	cube2.SetPos({ 10.0f,5.0f,14.0f });
	nano.SetRootTransform(
		dx::XMMatrixRotationY(PI / 2.f) *
		dx::XMMatrixTranslation(27.f, -0.56f, 1.7f)
	);
	goblin.SetRootTransform(
		dx::XMMatrixRotationY(-PI / 2.f) *
		dx::XMMatrixTranslation(-8.f, 10.f, 0.f)
	);
	*/
	cube.LinkTechniques(rg);
	//cube2.LinkTechniques(rg);
	light.LinkTechniques(rg);
	//sponza.LinkTechniques(rg);
	//goblin.LinkTechniques(rg);
	//nano.LinkTechniques(rg);
	cameras.LinkTechniques(rg);

	rg.BindShadowCamera(*light.ShareCamera());

}


void App::HandleInput(float dt)
{
	
	while (const auto e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}
		switch (e->GetCode())
		{
			case VK_ESCAPE:
			
				if (wnd.CursorEnabled())
				{
					wnd.DisableCursor();
					wnd.mouse.EnableRawInput();

				}
				else
				{
					wnd.EnableCursor();
					wnd.mouse.DisableRawInput();
				}
				break;
			case VK_F1:
				showDemoWindow = true;
				break;
			case VK_RETURN:
				savingDepth = true;
				break;
		}
	
	}

	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cameras->Translate({ 0.f, 0.f, dt });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cameras->Translate({ -dt, 0.f, 0.f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cameras->Translate({ 0.f, 0.f, -dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cameras->Translate({ dt, 0.f, 0.f });
		}
		if (wnd.kbd.KeyIsPressed('R'))
		{
			cameras->Translate({ 0.f, dt, 0.f });
		}
		if (wnd.kbd.KeyIsPressed('F'))
		{
			cameras->Translate({ 0.f, -dt, 0.f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cameras->Rotate((float)delta->x, (float)delta->y);
		}
	}	
	
}
void App::DoFrame(float dt)
{
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	light.Bind(wnd.Gfx(), cameras->GetMatrix());
	rg.BindMainCamera(cameras.GetActiveCamera());

	light.Submit(Channel::main);
	cube.Submit(Channel::main);
	/*sponza.Submit(Channel::main);
	cube2.Submit(Channel::main);
	goblin.Submit(Channel::main);
	nano.Submit(Channel::main);*/
	cameras.Submit(Channel::main);

	//sponza.Submit(Channel::shadow);
	cube.Submit(Channel::shadow);
	/*sponza.Submit(Channel::shadow);
	cube2.Submit(Channel::shadow);
	goblin.Submit(Channel::shadow);
	nano.Submit(Channel::shadow);*/

	rg.Execute(wnd.Gfx());

	if (savingDepth)
	{
		rg.DumpShadowMap(wnd.Gfx(), "shadow.png");
		savingDepth = false;
	}

	// imgui windows
	static MP sponzaProbe{"Sponza"};
	static MP goblinProbe{"Goblin"};
	static MP nanoProbe{"Nano"};
	//sponzaProbe.SpawnWindow(sponza);
	//goblinProbe.SpawnWindow(goblin);
	//nanoProbe.SpawnWindow(nano);
	cameras.SpawnWindow(wnd.Gfx());
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	cube.SpawnControlWindow(wnd.Gfx(), "Cube 1");
	//cube2.SpawnControlWindow(wnd.Gfx(), "Cube 2");

	rg.RenderWindows(wnd.Gfx());

	// present
	wnd.Gfx().EndFrame();
	rg.Reset();

}

void App::ShowImguiDemoWindow()
{
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}


App::~App()
{
}

int App::Go()
{
	while (true)
	{
		if (const std::optional<int> ecode = Window::ProcessMessages())
		{
			// if return optional has value , means we are quitting
			return *ecode;
		}
		// execute the frame logic
		const auto dt = timer.Mark() * speedFactor;
		HandleInput(dt);
		DoFrame(dt);

	}

}

