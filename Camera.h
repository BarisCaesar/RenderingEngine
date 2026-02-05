#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
private:
	DirectX::XMFLOAT3 pos;
	float xRotation;
	float yRotation;
	DirectX::XMFLOAT3 rot;
	static constexpr float travelSpeed = 12.f;
	static constexpr float rotationSpeed = 0.004f;
};