#include "App.h"
#include "Box.h"
#include "Cylinder.h"
#include "Pyramid.h"
#include "SkinnedBox.h"
#include "AssimpTest.h"
#include <memory>
#include <algorithm>
#include "RMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Vertex.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

void f()
{
	VertexBuffer vb(std::move(
		VertexLayout{}
		.Append<VertexLayout::Position3D>()
		.Append <VertexLayout::Normal>()
		.Append <VertexLayout::Texture2D>()
	));
	vb.EmplaceBack(
		dx::XMFLOAT3(1.f, 1.f, 5.f),
		dx::XMFLOAT3(2.f, 1.f, 4.f),
		dx::XMFLOAT2(6.f, 10.f)
	);
	vb.EmplaceBack(
		dx::XMFLOAT3(6.f, 10.f, 6.f),
		dx::XMFLOAT3(9.f, 7.f, 9.f),
		dx::XMFLOAT2(4.f, 10.f)
	);
	auto pos = vb[0].Attr<VertexLayout::Position3D>();
	auto normal = vb[0].Attr<VertexLayout::Normal>();
	auto texture = vb[0].Attr<VertexLayout::Texture2D>();
	vb.Back().Attr<VertexLayout::Position3D>().z = 420.f;
	pos = vb.Back().Attr<VertexLayout::Position3D>();
	const auto& cvb = vb;
	pos = cvb[1].Attr<VertexLayout::Position3D>();
}

App::App()
	:
	wnd(800, 600, "Basic App"),
	light(wnd.Gfx())
{
	f();
	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{
		}
		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 mat = { cdist(rng), cdist(rng), cdist(rng) };
			switch (sdist(rng))
			{
			case 0:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist, mat
				);
			case 1:
				return std::make_unique<Cylinder>(
					gfx, rng, adist, ddist, odist,
					rdist, bdist, tdist
				);
			case 2:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist, odist,
					rdist, tdist
				);
			case 3:
				return std::make_unique<SkinnedBox>(
					gfx, rng, adist, ddist, odist,
					rdist
				);
			case 4:
				return std::make_unique<AssimpTest>(
					gfx, rng,
					adist, ddist, odist, rdist, 
					mat, 1.5f
				);
			default:
				assert(false && "impossible drawable option in factory");
				return {};
			}
			
			
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> sdist{ 0, 4 };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
		std::uniform_int_distribution<int> tdist{ 3,30 };
		
	};


	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, Factory{wnd.Gfx()});

	// init box pointers for editing instance parameters
	for (auto& pDrawable : drawables)
	{
		if (auto pRawDrawable = dynamic_cast<Box*>(pDrawable.get()))
		{
			boxes.push_back(pRawDrawable);
		}
	}

	const auto s = Surface::FromFile("Images//kappa50.png");

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	
}


void App::DoFrame()
{
	auto dt = timer.Mark() * speedFactor;


	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	// render geometry
	for (auto& d : drawables)
	{
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(wnd.Gfx());
	}
	light.Draw(wnd.Gfx());

	static char buffer[1024];
	
	// imgui windows
	SpawnSimulationWindow();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	SpawnBoxWindowManagerWindow();
	SpawnBoxWindows();
	
	// present
	wnd.Gfx().EndFrame();
}

void App::SpawnSimulationWindow() noexcept
{
	// imgui window to control simulation speed
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speedFactor, 0.f, 4.f);
		ImGui::Text("%.3f ms/frame (%.1f FPS", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING");
	}
	ImGui::End();
}

void App::SpawnBoxWindowManagerWindow() noexcept
{
	//imgui window to open box windows
	if (ImGui::Begin("Boxes"))
	{
		using namespace std::string_literals;
		const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;
		if (ImGui::BeginCombo("Box Number", preview.c_str()))
		{
			for (int i = 0; i < boxes.size(); i++)
			{
				const bool selected = comboBoxIndex ? (*comboBoxIndex == i) : false;
				if (ImGui::Selectable(std::to_string(i).c_str(), selected))
				{
					comboBoxIndex = i;
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}

			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
		{
			boxControlIds.insert(*comboBoxIndex);
			comboBoxIndex.reset();
		}
	}
	ImGui::End();
}

void App::SpawnBoxWindows() noexcept
{
	// imgui window to adjust box instance parameters
	for (auto i = boxControlIds.begin(); i != boxControlIds.end();)
	{
		if (!boxes[*i]->SpawnControlWindow(*i, wnd.Gfx()))
		{
			i = boxControlIds.erase(i);
		}
		else
		{
			i++;
		}
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

