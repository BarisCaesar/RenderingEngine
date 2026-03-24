#pragma once
#include <array>
#include "BindableCommon.h"
#include "Graphics.h"
#include "Job.h"
#include "Pass.h"
#include "PerfLog.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include <array>

class FrameCommander
{
public:
	FrameCommander(Graphics& gfx)
		:
		depthStencil(gfx, gfx.GetWidth(), gfx.GetHeight()),
		renderTarget(gfx, gfx.GetWidth(), gfx.GetHeight())
	{
		namespace dx = DirectX;

		// setup for fullscreen geometry
		DynamicVertex::VertexLayout layout;
		layout.Append(DynamicVertex::VertexLayout::Position2D);
		DynamicVertex::VertexBuffer bufFull{ layout };
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,-1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,-1 });
		pVertexBufferFull = Bind::VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull));
		std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
		pIndexBufferFull = Bind::IndexBuffer::Resolve(gfx, "$Full", std::move(indices));

		// setup fullscreen shaders
		pPixelShaderFull = Bind::PixelShader::Resolve(gfx, "BlurOutline_PS.cso");
		pVertexShaderFull = Bind::VertexShader::Resolve(gfx, "Fullscreen_VS.cso");
		pLayoutFull = Bind::InputLayout::Resolve(gfx, layout, pVertexShaderFull->GetBytecode());
		pSamplerFull = Bind::Sampler::Resolve(gfx, false, true);
		pBlenderFull = Bind::Blender::Resolve(gfx, true);
	}
	void Accept(Job job, size_t target) noexcept
	{
		passes[target].Accept(job);
	}
	void Execute(Graphics& gfx) const noxnd
	{
		using namespace Bind;


		// setup render target used for normal passes
		depthStencil.Clear(gfx);
		renderTarget.Clear(gfx);
		gfx.BindSwapBuffer(depthStencil);
		// main phong lighting pass
		Blender::Resolve(gfx, false)->Bind(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
		passes[0].Execute(gfx);
		// outline masking pass
		Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
		NullPixelShader::Resolve(gfx)->Bind(gfx);
		passes[1].Execute(gfx);
		// outline drawing pass
		renderTarget.BindAsTarget(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
		passes[2].Execute(gfx);
		// fullscreen blur + blend pass
		gfx.BindSwapBuffer(depthStencil);
		renderTarget.BindAsTexture(gfx, 0);
		pVertexBufferFull->Bind(gfx);
		pIndexBufferFull->Bind(gfx);
		pVertexShaderFull->Bind(gfx);
		pPixelShaderFull->Bind(gfx);
		pLayoutFull->Bind(gfx);
		pSamplerFull->Bind(gfx);
		pBlenderFull->Bind(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
		gfx.DrawIndexed(pIndexBufferFull->GetCount());
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
	RenderTarget renderTarget;
	std::shared_ptr<Bind::VertexBuffer> pVertexBufferFull;
	std::shared_ptr<Bind::IndexBuffer> pIndexBufferFull;
	std::shared_ptr<Bind::VertexShader> pVertexShaderFull;
	std::shared_ptr<Bind::PixelShader> pPixelShaderFull;
	std::shared_ptr<Bind::InputLayout> pLayoutFull;
	std::shared_ptr<Bind::Sampler> pSamplerFull;
	std::shared_ptr<Bind::Blender> pBlenderFull;
};
