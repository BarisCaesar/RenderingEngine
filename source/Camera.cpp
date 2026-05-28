#include "Camera.h"
#include "imgui/imgui.h"
#include "RMath.h"
#include "Graphics.h"


namespace dx = DirectX;
Camera::Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos, float homeXRotation, float homeYRotation) noexcept
	:
	name(std::move(name)),
	homePos(homePos),
	homeXRotation(homeXRotation),
	homeYRotation(homeYRotation),
	proj(1.f, 9.f / 16.f, 0.5f, 400.f),
	indicator(gfx)
{
	Reset();
	indicator.SetPos(pos);
	indicator.SetRotation({ xRotation,yRotation,0.0f });
}
void Camera::BindToGraphics(Graphics& gfx) const
{
	gfx.SetCamera(GetMatrix());
	gfx.SetProjection(proj.GetMatrix());
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

void Camera::SpawnControlWidgets() noexcept
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
	proj.RenderWidgets();
}

void Camera::Reset() noexcept
{
	pos = homePos;
	xRotation = homeXRotation;
	yRotation = homeYRotation;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yRotation = wrap_angle(yRotation + dx * rotationSpeed);
	xRotation = std::clamp(xRotation + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
	indicator.SetRotation({ xRotation,yRotation,0.0f });

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
	indicator.SetPos(pos);
}

DirectX::XMFLOAT3 Camera::GetPos() const noexcept
{
	return pos;
}

const std::string& Camera::GetName() const noexcept
{
	return name;
}

void Camera::LinkTechniques(RenderGraph::RenderGraph& rg)
{
	indicator.LinkTechniques(rg);
}

void Camera::Submit() const
{
	indicator.Submit();
}
