#pragma once
#include "Pass.h"

namespace Bind
{
	class BufferResource;
}

namespace RenderGraph
{
	class BufferClearPass : public Pass
	{
	public:
		BufferClearPass(std::string name);
		void Execute(Graphics& gfx) const noxnd override;
	private:
		std::shared_ptr<Bind::BufferResource> buffer;
	};
}