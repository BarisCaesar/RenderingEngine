#pragma once
#include "BindableCommon.h"
#include "RMath.h"
#include "imgui/imgui.h"

class BlurPack
{
public:
	BlurPack(Graphics& gfx, int radius = 7, float sigma = 2.6f)
		:
		shader(gfx, "Blur_PS.cso"),
		kernelConstBuf(gfx, 0u),
		controlConstBuf(gfx, 1u),
		radius(radius),
		sigma(sigma)
	{
		SetKernelGauss(gfx, radius, sigma);
	}
	void Bind(Graphics& gfx) noexcept
	{
		shader.Bind(gfx);
		kernelConstBuf.Bind(gfx);
		controlConstBuf.Bind(gfx);
	}
	void SetHorizontal(Graphics& gfx)
	{
		controlConstBuf.Update(gfx, { TRUE });
	}
	void SetVertical(Graphics& gfx)
	{
		controlConstBuf.Update(gfx, { FALSE });
	}
	void ShowWindow(Graphics& gfx)
	{
		ImGui::Begin("Blur");

		bool filterChanged = false;
		{
			const char* items[] = { "Gauss", "Box" };
			static const char* curItem = items[0];
			if (ImGui::BeginCombo("Filter Type", curItem))
			{
				for (int n = 0; n < std::size(items); n++)
				{
					const bool isSelected = (curItem == items[n]);
					if (ImGui::Selectable(items[n], isSelected))
					{
						filterChanged = true;
						curItem = items[n];
						if (curItem == items[0])
						{
							kernelType = KernelType::Gauss;
						}
						else if (curItem == items[1])
						{
							kernelType = KernelType::Box;
						}
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		bool radChange = ImGui::SliderInt("Radius", &radius, 0, 15);
		bool sigChange = ImGui::SliderFloat("Sigma", &sigma, 0.1f, 10.f);
		if (radChange || sigChange || filterChanged)
		{
			if (kernelType == KernelType::Gauss)
			{
				SetKernelGauss(gfx, radius, sigma);
			}
			else if (kernelType == KernelType::Box)
			{
				SetKernelBox(gfx, radius);
			}
		}
		ImGui::End();
	}
	
	// for more accurate coefs, need to integrate
	void SetKernelGauss(Graphics& gfx, int radius, float sigma) noxnd
	{
		assert(radius <= maxRadius);
		Kernel k;
		k.nTaps = radius * 2 + 1;
		float sum = 0.f;
		for (int i = 0; i < k.nTaps; i++)
		{
			const auto x = float(i - radius);
			const auto g = gauss(x, sigma);
			sum += g;
			k.coefficients[i].x = g;
		}
		for (int i = 0; i < k.nTaps; i++)
		{
			k.coefficients[i].x /= sum;
		}
		kernelConstBuf.Update(gfx, k);
	}
	void SetKernelBox(Graphics& gfx, int radius) noxnd
	{
		assert(radius <= maxRadius);
		Kernel k;
		k.nTaps = radius * 2 + 1;
		const float c = 1.f / k.nTaps;
		for (int i = 0; i < k.nTaps; i++)
		{
			k.coefficients[i].x = c;
		}
		kernelConstBuf.Update(gfx, k);
	}


private:
	enum class KernelType
	{
		Gauss,
		Box,
	};
	static constexpr int maxRadius = 15;
	int radius;
	float sigma;
	KernelType kernelType = KernelType::Gauss;
	struct Kernel
	{
		int nTaps;
		float padding[3];
		DirectX::XMFLOAT4 coefficients[maxRadius * 2 + 1];
	};
	struct Control
	{
		BOOL horizontal;
		float padding[3];
	};
	Bind::PixelShader shader;
	Bind::PixelConstantBuffer<Kernel> kernelConstBuf;
	Bind::PixelConstantBuffer<Control> controlConstBuf;
};