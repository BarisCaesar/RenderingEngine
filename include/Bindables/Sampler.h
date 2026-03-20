#pragma once
#include "Bindable.h"


namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler(Graphics& gfx, bool anisotropicEnable, bool reflect);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Sampler> Resolve(Graphics& gfx, bool anisotropicEnable = true, bool reflect = false);
		static std::string GenerateUID(bool anisotropicEnable, bool reflect);
		std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
		bool anisotropicEnable;
		bool reflect;
	};
}
