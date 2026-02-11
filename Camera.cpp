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
	using namespace dx;
	const XMVECTOR forwardBaseVector = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	const auto lookVector = XMVector3Transform(forwardBaseVector,
		XMMatrixRotationRollPitchYaw(xRotation, yRotation, 0.f)
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and directon
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3(&pos);
	const auto camTarget = camPosition + lookVector;
	return XMMatrixLookAtLH(camPosition, camTarget, XMVectorSet(0.f, 1.f, 0.f, 0.f));
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
		ImGui::SliderAngle("Rotation X", &xRotation, 0.995 * -90.0f, 0.995 * 90.0f);
		ImGui::SliderAngle("Rotation Y", &yRotation, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	pos = { -10.0f,0.0f,-20.0f };
	xRotation = 0.0f;
	yRotation = 0.45f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yRotation = wrap_angle(yRotation + dx * rotationSpeed);
	xRotation = std::clamp(xRotation + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);

}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	dx::XMStoreFloat3(&translation, dx::XMVector3Transform(
		dx::XMLoadFloat3(&translation),
		dx::XMMatrixRotationRollPitchYaw(xRotation, yRotation, 0.f) *
		dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
	));
	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z
	};
}
