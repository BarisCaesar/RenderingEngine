#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include "DirectXMath.h"

namespace Bind
{
	class TransformCBuf : public CloningBindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};
	public:
		TransformCBuf(Graphics& gfx, UINT slot = 0u);
		void Bind(Graphics& gfx) noexcept override;
		void InitializeParentReference(const Drawable& parent) noexcept override;
		std::unique_ptr<CloningBindable> Clone() const noexcept override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& transforms) noexcept;
		Transforms GetTransforms(Graphics& gfx) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable* pParent = nullptr;
	};
}

