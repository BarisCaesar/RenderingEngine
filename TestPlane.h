#pragma once
#include "Drawable.h"

class TestPlane : public Drawable
{
public:
	TestPlane(Graphics& gfx, float size, DirectX::XMFLOAT4 color = {1.f, 1.f, 1.f, 0.f});
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float xRotation, float yRotation, float zRotation) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow(Graphics& gfx) noexcept;
private:
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT4 color;
	}pmc;
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	float xRotation = 0.0f;
	float yRotation = 0.0f;
	float zRotation = 0.0f;
};