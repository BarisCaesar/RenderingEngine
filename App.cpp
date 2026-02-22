#include "App.h"
#include <memory>
#include <algorithm>
#include "RMath.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "RUtil.h"
#include "DynamicConstant.h"
#include <cstring>

namespace dx = DirectX;

void TestDynamicConstant()
{
	using namespace std::string_literals;
	// data control tests
	{
		DynamicConstBuf::Layout s;

		s.Add<DynamicConstBuf::Struct>("structboi"s);
		s["structboi"].Add<DynamicConstBuf::Float3>("float3boi"s);
		s["structboi"].Add<DynamicConstBuf::Float>("floatboi"s);
		s.Add<DynamicConstBuf::Float>("woot"s);
		s.Add<DynamicConstBuf::Array>("arr"s);
		s["arr"s].Set<DynamicConstBuf::Struct>(4);
		s["arr"s].T().Add<DynamicConstBuf::Float3>("twerk"s);
		s["arr"s].T().Add<DynamicConstBuf::Array>("werk"s);
		s["arr"s].T()["werk"s].Set<DynamicConstBuf::Float>(6);
		s["arr"s].T().Add<DynamicConstBuf::Array>("meta"s);
		s["arr"s].T()["meta"s].Set<DynamicConstBuf::Array>(6);
		s["arr"s].T()["meta"s].T().Set<DynamicConstBuf::Matrix>(4);
		s["arr"s].T().Add<DynamicConstBuf::Bool>("booler"s);
		DynamicConstBuf::Buffer b(s);
		{
			auto exp = 42.0f;
			b["woot"s] = exp;
			float act = b["woot"s];
			assert(act == exp);
		}
		{
			auto exp = 420.0f;
			b["structboi"s]["floatboi"s] = exp;
			float act = b["structboi"s]["floatboi"s];
			assert(act == exp);
		}
		{
			auto exp = 111.0f;
			b["arr"s][2]["werk"s][5] = exp;
			float act = b["arr"s][2]["werk"s][5];
			assert(act == exp);
		}
		{
			auto exp = DirectX::XMFLOAT3{ 69.0f,0.0f,0.0f };
			b["structboi"s]["float3boi"s] = exp;
			dx::XMFLOAT3 act = b["structboi"s]["float3boi"s];
			assert(!std::memcmp(&exp, &act, sizeof(DirectX::XMFLOAT3)));
		}
		{
			DirectX::XMFLOAT4X4 exp;
			dx::XMStoreFloat4x4(
				&exp,
				dx::XMMatrixIdentity()
			);
			b["arr"s][2]["meta"s][5][3] = exp;
			dx::XMFLOAT4X4 act = b["arr"s][2]["meta"s][5][3];
			assert(!std::memcmp(&exp, &act, sizeof(DirectX::XMFLOAT4X4)));
		}
		{
			auto exp = true;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert(act == exp);
		}
		{
			auto exp = false;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert(act == exp);
		}
		// exists
		{
			assert(b["structboi"s]["float3boi"s].Exists());
			assert(!b["structboi"s]["fubar"s].Exists());
			if (auto opt = b["structboi"s]["float3boi"s].Exists())
			{
				auto& ref = *opt;
				dx::XMFLOAT3 f = ref;
				assert(f.x == 69.0f);
			}
		}
		const auto& cb = b;
		{
			dx::XMFLOAT4X4 act = cb["arr"s][2]["meta"s][5][3];
			assert(act._11 == 1.0f);
		}
		// this doesn't compile: buffer is const
		// cb["arr"][2]["booler"] = true;

		// this fails assertion: array out of bounds
		// cb["arr"s][200];
	}
	// size test array of arrays
	{
		DynamicConstBuf::Layout s;
		s.Add<DynamicConstBuf::Array>("arr");
		s["arr"].Set<DynamicConstBuf::Array>(6);
		s["arr"].T().Set<DynamicConstBuf::Matrix>(4);
		DynamicConstBuf::Buffer b(s);

		auto act = b.GetSizeInBytes();
		assert(act == 16u * 4u * 4u * 6u);
	}
	// size test array of structs with padding
	{
		DynamicConstBuf::Layout s;
		s.Add<DynamicConstBuf::Array>("arr");
		s["arr"].Set<DynamicConstBuf::Struct>(6);
		s["arr"s].T().Add<DynamicConstBuf::Float2>("a");
		s["arr"].T().Add<DynamicConstBuf::Float3>("b"s);
		DynamicConstBuf::Buffer b(s);

		auto act = b.GetSizeInBytes();
		assert(act == 16u * 2u * 6u);
	}
	// size test array of primitive that needs padding
	{
		DynamicConstBuf::Layout s;
		s.Add<DynamicConstBuf::Array>("arr");
		s["arr"].Set<DynamicConstBuf::Float3>(6);
		DynamicConstBuf::Buffer b(s);

		auto act = b.GetSizeInBytes();
		assert(act == 16u * 6u);
	}
}

App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1280, 720, "Basic App"),
	scriptCommander(TokenizeQuoted(commandLine)),
	light(wnd.Gfx())
{
	
	TestDynamicConstant();
	//wall.SetRootTransform(dx::XMMatrixTranslation(-1.5f, 0.0f, 0.0f));
	//plane.SetPos({ 12.0f,0.0f,0.0f });
	//goblin.SetRootTransform(dx::XMMatrixTranslation(0.f, 0.f, -4.f));
	//nano.SetRootTransform(dx::XMMatrixTranslation(0.f, -7.f, 6.f));
	bluePlane.SetPos(cam.GetPos());
	redPlane.SetPos(cam.GetPos());

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 400.0f));
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
	//goblin.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	sponza.Draw(wnd.Gfx());
	bluePlane.Draw(wnd.Gfx());
	redPlane.Draw(wnd.Gfx());

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

	//goblin.ShowWindow(wnd.Gfx(), "Goblin");
	//wall.ShowWindow(wnd.Gfx(), "Wall");
	//plane.SpawnControlWindow(wnd.Gfx());
	//nano.ShowWindow(wnd.Gfx(), "Model 1");
	sponza.ShowWindow(wnd.Gfx(), "Sponza");
	bluePlane.SpawnControlWindow(wnd.Gfx(), "Blue Plane");
	redPlane.SpawnControlWindow(wnd.Gfx(), "Red Plane");

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

