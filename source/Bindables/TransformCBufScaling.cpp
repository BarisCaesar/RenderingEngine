#include "TransformCBufScaling.h"
#include "TechniqueProbe.h"

namespace dx = DirectX;

Bind::TransformCBufScaling::TransformCBufScaling(Graphics& gfx, float scale)
	:
	TransformCBuf(gfx),
	buf(MakeLayout())
{
	buf["scale"] = scale;
}

void Bind::TransformCBufScaling::Accept(TechniqueProbe& probe)
{
	probe.VisitBuffer(buf);
}

void Bind::TransformCBufScaling::Bind(Graphics& gfx) noexcept
{
	const float scale = buf["scale"];
	const auto scaleMatrix = dx::XMMatrixScaling(scale, scale, scale);
	auto xf = GetTransforms(gfx);
	xf.modelView = xf.modelView * scaleMatrix;
	xf.modelViewProj = xf.modelViewProj * scaleMatrix;
	UpdateBindImpl(gfx, xf);
}

DynamicConstBuf::RawLayout Bind::TransformCBufScaling::MakeLayout()
{
	DynamicConstBuf::RawLayout layout;
	layout.Add<DynamicConstBuf::Float>("scale");
	return layout;
}