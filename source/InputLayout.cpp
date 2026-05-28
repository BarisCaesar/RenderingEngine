#include "InputLayout.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "Vertex.h"
#include "VertexShader.h"

namespace Bind
{
	InputLayout::InputLayout(Graphics& gfx,
		DynamicVertex::VertexLayout layout_in,
		const VertexShader& vs)
		:
		layout(std::move(layout_in)),
		vertexShaderUID(vs.GetUID())
		
	{
		INFOMAN(gfx);

		const auto d3dLayout = layout.GetD3DLayout();
		const auto pByteCode = vs.GetBytecode();

		GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
			d3dLayout.data(), (UINT)d3dLayout.size(),
			pByteCode->GetBufferPointer(),
			pByteCode->GetBufferSize(),
			&pInputLayout
		));
	}
	const DynamicVertex::VertexLayout InputLayout::GetLayout() const noexcept
	{
		return layout;
	}

	void InputLayout::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetInputLayout(pInputLayout.Get()));
	}
	std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx,
		const DynamicVertex::VertexLayout& layout, const VertexShader& vs)
	{
		return Codex::Resolve<InputLayout>(gfx, layout, vs);
	}
	std::string InputLayout::GenerateUID(const DynamicVertex::VertexLayout& layout, const VertexShader& vs)
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vs.GetUID();
	}
	std::string InputLayout::GetUID() const noexcept
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vertexShaderUID;
	}
}

