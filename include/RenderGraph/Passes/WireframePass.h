#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "Source.h"
#include "Sink.h"
#include "BindableCommon.h"

class Graphics;

namespace RenderGraph
{
	class WireframePass : public RenderQueuePass
	{
	public:
		WireframePass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			using namespace Bind;
			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::DepthOff));
		}
	};
}