#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera(DirectX::XMFLOAT3 homePos = { 0.0f,0.0f,0.0f }, float homeXRotation = 0.0f, float homeYRotation = 0.0f) noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;
private:
	DirectX::XMFLOAT3 homePos;
	float homeXRotation;
	float homeYRotation;
	DirectX::XMFLOAT3 pos;
	float xRotation;
	float yRotation;
	static constexpr float travelSpeed = 12.f;
	static constexpr float rotationSpeed = 0.004f;
};