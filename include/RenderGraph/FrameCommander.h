#pragma once
#include <array>
#include <optional>
#include "Pass.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "BindableCommon.h"
#include "Graphics.h"
#include "Job.h"
#include "ConditionalNoexcept.h"
#include "BlurPack.h"

namespace Bind
{
	class VertexBuffer;
	class IndexBuffer;
	class VertexShader;
	class InputLayout;
	class Sampler;
	class Blender;
}

class FrameCommander
{
public:
	FrameCommander(Graphics& gfx);
	void Accept(Job job, size_t target) noexcept;
	void Execute(Graphics& gfx) noxnd;
	void Reset() noexcept;
	void ShowWindows(Graphics& gfx);
private:
	std::array<Pass, 3> passes;
	int downFactor = 1;
	DepthStencil depthStencil;
	std::optional<RenderTarget> renderTarget1;
	std::optional<RenderTarget> renderTarget2;
	BlurPack blur;
	std::shared_ptr<Bind::VertexBuffer> pVertexBufferFull;
	std::shared_ptr<Bind::IndexBuffer> pIndexBufferFull;
	std::shared_ptr<Bind::VertexShader> pVertexShaderFull;
	std::shared_ptr<Bind::InputLayout> pLayoutFull;
	std::shared_ptr<Bind::Sampler> pSamplerFullPoint;
	std::shared_ptr<Bind::Sampler> pSamplerFullBilinear;
	std::shared_ptr<Bind::Blender> pBlenderMerge;
};
