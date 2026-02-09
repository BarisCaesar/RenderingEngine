#include "TransformCBufExtended.h"

namespace Bind
{
	TransformCBufExtended::TransformCBufExtended(Graphics& gfx, const Drawable& parent, UINT slotVS, UINT slotPS)
		:
		TransformCBuf(gfx, parent, slotVS)
	{
		if (!pPcbuf)
		{
			pPcbuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotPS);
		}
	}
	void TransformCBufExtended::Bind(Graphics& gfx) noexcept
	{
		const auto transforms = TransformCBuf::GetTransforms(gfx);
		TransformCBuf::UpdateBindImpl(gfx, transforms);
		UpdateBindImpl(gfx, transforms);
	}
	void TransformCBufExtended::UpdateBindImpl(Graphics& gfx, const Transforms& transforms) noexcept
	{
		pPcbuf->Update(gfx, transforms);
		pPcbuf->Bind(gfx);
	}

	std::unique_ptr<PixelConstantBuffer<TransformCBuf::Transforms>> TransformCBufExtended::pPcbuf;
}

