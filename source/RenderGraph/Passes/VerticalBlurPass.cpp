#include "VerticalBlurPass.h"
#include "Sink.h"
#include "Source.h"
#include "PixelShader.h"
#include "Blender.h"
#include "Stencil.h"
#include "Sampler.h"

namespace RenderGraph
{
	VerticalBlurPass::VerticalBlurPass(std::string name, Graphics& gfx)
		:
		FullscreenPass(std::move(name), gfx)
	{
		AddBind(Bind::PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
		AddBind(Bind::Blender::Resolve(gfx, true));
		AddBind(Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Mask));
		AddBind(Bind::Sampler::Resolve(gfx, Bind::Sampler::Type::Bilinear, true));

		AddBindSink<Bind::RenderTarget>("scratchIn");
		AddBindSink<Bind::CachingPixelConstantBufferEx>("kernel");
		RegisterSink(DirectBindableSink<Bind::CachingPixelConstantBufferEx>::Make("direction", direction));
		RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));

		RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
	}

	// see the note on HorizontalBlurPass::Execute
	void VerticalBlurPass::Execute(Graphics& gfx) const noxnd
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = false;
		direction->SetBuffer(buf);

		direction->Bind(gfx);
		FullscreenPass::Execute(gfx);
	}
}