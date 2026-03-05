#pragma once
#include "Drawable.h"
#include "Bindable.h"
#include "IndexBuffer.h"

class TestCube : public Drawable
{
public:
	TestCube(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float xRotation, float yRotation, float zRotation) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow(Graphics& gfx, const char* name) noexcept;
private:
	DirectX::XMFLOAT3 pos = { 1.f, 1.f, 1.f };
	float xRotation = 0.f;
	float yRotation = 0.f;
	float zRotation = 0.f;

};