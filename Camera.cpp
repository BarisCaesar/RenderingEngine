#include "Camera.h"
#include "imgui/imgui.h"
#include "RMath.h"


namespace dx = DirectX;
Camera::Camera() noexcept
{
	Reset();
}
DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	return dx::XMMatrixTranslation(-pos.x, -pos.y, -pos.z) *
		dx::XMMatrixRotationRollPitchYaw(-xRotation, -zRotation, 0.0f);
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Rotation X", &xRotation, -90.0f, 90.0f);
		ImGui::SliderAngle("Rotation Z", &zRotation, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	pos = { 0.0f,7.5f,-18.0f };
	xRotation = 0.0f;
	zRotation = 0.0f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	zRotation = wrap_angle(zRotation + dx * rotationSpeed);
	xRotation = std::clamp(xRotation + dy * rotationSpeed, -PI / 2.0f, PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	dx::XMStoreFloat3(&translation, dx::XMVector3Transform(
		dx::XMLoadFloat3(&translation),
		dx::XMMatrixRotationRollPitchYaw(xRotation, zRotation, 0.f) *
		dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
	));
	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z
	};
}
