#pragma once
#include <vector>
#include <memory>
#include "Bindable.h"
#include "Graphics.h"

class TechniqueProbe;
class Drawable;

namespace RenderGraph
{
	class RenderQueuePass;
	class RenderGraph;
}

class Step
{
public:
	Step(std::string targetPassName);
	Step(Step&&) = default;
	Step(const Step& src) noexcept;
	Step& operator=(const Step&) = delete;
	Step& operator=(Step&&) = delete;
	void AddBindable(std::shared_ptr<Bind::Bindable> bind_in);
	void Submit(const Drawable& drawable) const;
	void Bind(Graphics& gfx) const noxnd;
	void InitializeParentReferences(const Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe);
	void Link(RenderGraph::RenderGraph& rg);
private:
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
	RenderGraph::RenderQueuePass* pTargetPass = nullptr;
	std::string targetPassName;
};