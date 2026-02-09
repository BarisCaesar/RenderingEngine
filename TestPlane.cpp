#include "TestPlane.h"
#include "Plane.h"
#include "BindableCommon.h"

TestPlane::TestPlane(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));
	const auto geometryTag = "$plane." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	AddBind(Texture::Resolve(gfx, "Images\\brickwall.jpg"));

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PhongPS.cso"));

	struct PSMaterialConstant
	{
		float specularIntensity = 0.8f;
		float specularPower = 45.0f;
		float padding[2];
	} pmc;
	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCBuf>(gfx, *this));
}

void TestPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void TestPlane::SetRotation(float xRotation, float yRotation, float zRotation) noexcept
{
	this->xRotation = xRotation;
	this->yRotation = yRotation;
	this->zRotation = zRotation;
}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(xRotation, yRotation, zRotation)*
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
