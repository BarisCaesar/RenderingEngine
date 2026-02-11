#include "App.h"
#include <memory>
#include <algorithm>
#include "RMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"

namespace dx = DirectX;

GDIPlusManager gdipm;


App::App()
	:
	wnd(1280, 720, "Basic App"),
	light(wnd.Gfx())
{
	//wall.SetRootTransform(dx::XMMatrixTranslation(-1.5f, 0.0f, 0.0f));
	//plane.SetPos({ 1.5f,0.0f,0.0f });
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f));
}


void App::DoFrame()
{
	auto dt = timer.Mark() * speedFactor;


	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	//wall.Draw(wnd.Gfx());
	//plane.Draw(wnd.Gfx());
	//nano.Draw(wnd.Gfx());
	goblin.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());

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
		}
	
	}

	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cam.Translate({ 0.f, 0.f, dt });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate({ -dt, 0.f, 0.f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate({ 0.f, 0.f, -dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cam.Translate({ dt, 0.f, 0.f });
		}
		if (wnd.kbd.KeyIsPressed('R'))
		{
			cam.Translate({ 0.f, dt, 0.f });
		}
		if (wnd.kbd.KeyIsPressed('F'))
		{
			cam.Translate({ 0.f, -dt, 0.f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cam.Rotate((float)delta->x, (float)delta->y);
		}
	}
	
	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();

	goblin.ShowWindow(wnd.Gfx(), "Goblin");
	//wall.ShowWindow("Wall");
	//plane.SpawnControlWindow(wnd.Gfx());
	//nano.ShowWindow("Model 1");

	// present
	wnd.Gfx().EndFrame();
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
		DoFrame();
	}

}

