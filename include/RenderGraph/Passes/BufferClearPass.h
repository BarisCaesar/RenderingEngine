#pragma once
#include "Pass.h"

namespace RenderGraph
{
	class BufferClearPass : public Pass
	{
	public:
		BufferClearPass(std::string name);
		void Execute(Graphics& gfx) const noxnd override;
	};
}