#include "App.h"

App::App()
	:
	wnd(800, 600, "Basic App")
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

void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.f + 0.5f;
	wnd.Gfx().ClearBuffer(c, c, 1.f);
	wnd.Gfx().DrawTestTriangle(timer.Peek(), (wnd.mouse.GetPosX() / 400.f) - 1.f, (- wnd.mouse.GetPosY() / 300.f) + 1.f);
	wnd.Gfx().DrawTestTriangle(timer.Peek(), 0.f, 0.f );
	wnd.Gfx().EndFrame();
}
