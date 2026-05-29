#pragma once
#include <DirectXMath.h>
#include <string>
#include "Projection.h"
#include "CameraIndicator.h"

class Graphics;
namespace RenderGraph
{
	class RenderGraph;
}

class Camera
{
public:
	Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos = { 0.0f,0.0f,0.0f }, float homeXRotation = 0.0f, float homeYRotation = 0.0f) noexcept;
	void BindToGraphics(Graphics& gfx) const;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWidgets(Graphics& gfx) noexcept;
	void Reset() noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;
	const std::string& GetName() const noexcept;
	void LinkTechniques(RenderGraph::RenderGraph& rg);
	void Submit() const;
private:
	std::string name;
	DirectX::XMFLOAT3 homePos;
	float homeXRotation;
	float homeYRotation;
	DirectX::XMFLOAT3 pos;
	float xRotation;
	float yRotation;
	static constexpr float travelSpeed = 12.f;
	static constexpr float rotationSpeed = 0.004f;
	Projection proj;
	CameraIndicator indicator;
};