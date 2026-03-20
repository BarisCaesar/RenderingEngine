#include "Sampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Sampler::Sampler(Graphics& gfx, bool anisotropicEnable, bool reflect)
		:
		anisotropicEnable(anisotropicEnable),
		reflect(reflect)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT{});
		samplerDesc.Filter = anisotropicEnable ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		samplerDesc.MipLODBias = 0.f;
		samplerDesc.MinLOD = 0.f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler));
	}

	void Sampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(0u, 1u, pSampler.GetAddressOf());
	}
	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx, bool anisotropicEnable, bool reflect)
	{
		return Codex::Resolve<Sampler>(gfx, anisotropicEnable, reflect);
	}
	std::string Sampler::GenerateUID(bool anisotropicEnable, bool reflect)
	{
		using namespace std::string_literals;
		return typeid(Sampler).name() + "#"s + (anisotropicEnable ? "A"s : "a"s) + (reflect ? "R"s : "W"s);
	}
	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID(anisotropicEnable, reflect);
	}
}

