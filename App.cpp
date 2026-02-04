#include "App.h"
#include <memory>
#include <algorithm>
#include "RMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

GDIPlusManager gdipm;


App::App()
	:
	wnd(1280, 720, "Basic App"),
	light(wnd.Gfx())
{

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f));
	
}


void App::DoFrame()
{
	auto dt = timer.Mark() * speedFactor;


	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	nano.Draw(wnd.Gfx());

	light.Draw(wnd.Gfx());

	while (const auto e = wnd.kbd.ReadKey())
	{
		if (e->IsPress() && e->GetCode() == VK_INSERT)
		{
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
		}
	}
	
	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	nano.ShowWindow();
	ShowRawInputWindow();
	
	// present
	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow()
{
	static bool show_demo_window = true;
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}
}

void App::ShowRawInputWindow()
{
	while (const auto d = wnd.mouse.ReadRawDelta())
	{
		x += d->x;
		y += d->y;
	}
	if (ImGui::Begin("Raw Input"))
	{
		ImGui::Text("Raw Input: X: %d, Y: %d", x, y);
		ImGui::Text("Cursor: %s", wnd.CursorEnabled() ? "enabled" : "disabled");
	}
	ImGui::End();
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

