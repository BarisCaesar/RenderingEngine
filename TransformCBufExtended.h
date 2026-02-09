#pragma once
#include "TransformCBuf.h"

namespace Bind
{
	class TransformCBufExtended : public TransformCBuf
	{
	public:
		TransformCBufExtended(Graphics& gfx, const Drawable& parent, UINT slotVS = 0u, UINT slotPS = 0u);
		void Bind(Graphics& gfx) noexcept override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& transforms) noexcept;
	private:
		static std::unique_ptr<PixelConstantBuffer<Transforms>> pPcbuf;
	};
}