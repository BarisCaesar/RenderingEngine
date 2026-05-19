#pragma once
#include "ConditionalNoexcept.h"

class Drawable;
class Graphics;
class Step;

namespace RenderGraph
{
	class Job
	{
	public:
		Job(const Step* pStep, const Drawable* pDrawable);
		void Execute(Graphics& gfx) const noxnd;
	private:
		const Drawable* pDrawable;
		const Step* pStep;
	};
}
