#pragma once
#include "TechniqueProbe.h"
namespace RenderGraph
{
	class RenderGraph;
}

class StepLinkingProbe : public TechniqueProbe
{
protected:
	void OnSetStep() override;
private:
	RenderGraph::RenderGraph& rg;
};