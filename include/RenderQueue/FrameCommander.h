#pragma once
#include <array>
#include "BindableCommon.h"
#include "Graphics.h"
#include "Job.h"
#include "Pass.h"
#include "PerfLog.h"
#include "DepthStencil.h"

class FrameCommander
{
public:
	FrameCommander(Graphics& gfx)
		:
		depthStencil(gfx, gfx.GetWidth(), gfx.GetHeight())
	{}
	void Accept(Job job, size_t target) noexcept
	{
		passes[target].Accept(job);
	}
	void Execute(Graphics& gfx) const noxnd
	{
		using namespace Bind;


		// setup render target used for all passes
		depthStencil.Clear(gfx);
		gfx.BindSwapBuffer(depthStencil);
		// main phong lighting pass
		Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
		passes[0].Execute(gfx);
		// outline masking pass
		Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
		NullPixelShader::Resolve(gfx)->Bind(gfx);
		passes[1].Execute(gfx);
		// outline drawing pass
		Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
		passes[2].Execute(gfx);
	}
	void Reset() noexcept
	{
		for (auto& p : passes)
		{
			p.Reset();
		}
	}
private:
	std::array<Pass, 3> passes;
	DepthStencil depthStencil;
};
