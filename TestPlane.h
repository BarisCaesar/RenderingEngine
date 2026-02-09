#pragma once
#include "Drawable.h"

class TestPlane : public Drawable
{
public:
	TestPlane(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float xRotation, float yRotation, float zRotation) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
	float xRotation = 0.0f;
	float yRotation = 0.0f;
	float zRotation = 0.0f;
};