#include "TestCube.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "TransformCBufExtended.h"
#include "imgui/imgui.h"
#include "Stencil.h"

TestCube::TestCube(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cube::MakeIndependentTextured();
	model.Transform(dx::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	const auto geometryTag = "$cube." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	AddBind(Texture::Resolve(gfx, "Images\\brickwall.jpg"));
	AddBind(Sampler::Resolve(gfx));

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PhongPS.cso"));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
	
	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	std::shared_ptr<TransformCBufExtended> pTBufExtended = std::make_shared<TransformCBufExtended>(gfx, *this, 0u, 2u);
	AddBind(pTBufExtended);

	AddBind(std::make_shared<Stencil>(gfx, Stencil::Mode::Write));

	outlineEffect.push_back(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	outlineEffect.push_back(IndexBuffer::Resolve(gfx, geometryTag, model.indices));
	pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
	pvsbc = pvs->GetBytecode();
	outlineEffect.push_back(std::move(pvs));
	outlineEffect.push_back(PixelShader::Resolve(gfx, "SolidPS.cso"));
	struct SolidColorBuffer
	{
		DirectX::XMFLOAT4 color = { 1.0f,0.4f,0.4f,1.0f };
	} scb;
	outlineEffect.push_back(PixelConstantBuffer<SolidColorBuffer>::Resolve(gfx, scb, 1u));
	outlineEffect.push_back(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
	outlineEffect.push_back(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	outlineEffect.push_back(std::move(pTBufExtended));
	outlineEffect.push_back(std::make_shared<Stencil>(gfx, Stencil::Mode::Mask));
}

void TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept
{

	this->pos = pos;
}

void TestCube::SetRotation(float xRotation, float yRotation, float zRotation) noexcept
{
	this->xRotation = xRotation;
	this->yRotation = yRotation;
	this->zRotation = zRotation;
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	auto xf =  DirectX::XMMatrixRotationRollPitchYaw(xRotation, yRotation, zRotation) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	if (outlining)
	{
		xf = DirectX::XMMatrixScaling(1.03f, 1.03f, 1.03f) * xf;
 	}
	return xf;
}

void TestCube::SpawnControlWindow(Graphics& gfx, const char* name) noexcept
{
	if (ImGui::Begin(name))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &xRotation, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &yRotation, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &zRotation, -180.0f, 180.0f);
		ImGui::Text("Shading");
		bool changed0 = ImGui::SliderFloat("Spec. Int.", &pmc.specularIntensity, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Power", &pmc.specularPower, 0.0f, 100.0f);
		bool checkState = pmc.normalMappingEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		if (changed0 || changed1 || changed2)
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
	}
	ImGui::End();
}
