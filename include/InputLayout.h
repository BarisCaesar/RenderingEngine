//#pragma once
//#include "Bindable.h"
//#include "Vertex.h"
//
//namespace Bind
//{
//	class InputLayout : public Bindable
//	{
//	public:
//		InputLayout(Graphics& gfx,
//			DynamicVertex::VertexLayout layout,
//			ID3DBlob* pVertexShaderBytecode);
//		void Bind(Graphics& gfx) noexcept override;
//		const DynamicVertex::VertexLayout GetLayout() const noexcept;
//		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx,
//			const DynamicVertex::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode);
//		static std::string GenerateUID(const DynamicVertex::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode = nullptr);
//		std::string GetUID() const noexcept override;
//	protected:
//		DynamicVertex::VertexLayout layout;
//		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
//
//	};
//}

#pragma once
#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{
	class InputLayout : public Bindable
	{
	public:
		InputLayout(Graphics& gfx,
			DynamicVertex::VertexLayout layout,
			ID3DBlob* pVertexShaderBytecode);
		void Bind(Graphics& gfx) noexcept override;
		const DynamicVertex::VertexLayout GetLayout() const noexcept;
		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx,
			const DynamicVertex::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode);
		static std::string GenerateUID(const DynamicVertex::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode = nullptr);
		std::string GetUID() const noexcept override;
	protected:
		DynamicVertex::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}
