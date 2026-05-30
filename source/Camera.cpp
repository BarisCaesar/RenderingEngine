#include "Camera.h"
#include "imgui/imgui.h"
#include "RMath.h"
#include "Graphics.h"


namespace dx = DirectX;
Camera::Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos, float homeXRotation, float homeYRotation, bool tethered) noexcept
	:
	name(std::move(name)),
	homePos(homePos),
	homeXRotation(homeXRotation),
	homeYRotation(homeYRotation),
	proj(gfx, 1.f, 9.f / 16.f, 0.5f, 400.f),
	indicator(gfx),
	tethered(tethered)
{
	if (tethered)
	{
		pos = homePos;
		indicator.SetPos(pos);
		proj.SetPos(pos);
	}
	Reset(gfx);
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
DirectX::XMMATRIX Camera::GetProjection() const noexcept
{
	return proj.GetMatrix();
}
void Camera::SpawnControlWidgets(Graphics& gfx) noexcept
{
	bool rotDirty = false;
	bool posDirty = false;
	const auto dcheck = [](bool d, bool& carry) {carry = carry || d; };
	if (!tethered)
	{
		ImGui::Text("Position");
		dcheck(ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f"), posDirty);
	}
	ImGui::Text("Orientation");
	dcheck(ImGui::SliderAngle("Rotation X", &xRotation, 0.995 * -90.0f, 0.995 * 90.0f), rotDirty);
	dcheck(ImGui::SliderAngle("Rotation Y", &yRotation, -180.0f, 180.0f), rotDirty);
	proj.RenderWidgets(gfx);
	ImGui::Checkbox("Camera Indicator", &enableCameraIndicator);
	ImGui::Checkbox("Frustum Indicator", &enableFrustumIndicator);
	if (ImGui::Button("Reset"))
	{
		Reset(gfx);
	}
	if (rotDirty)
	{
		const dx::XMFLOAT3 angles = { xRotation,yRotation,0.0f };
		indicator.SetRotation(angles);
		proj.SetRotation(angles);
	}
	if (posDirty)
	{
		indicator.SetPos(pos);
		proj.SetPos(pos);
	}
}

void Camera::Reset(Graphics& gfx) noexcept
{
	if (!tethered)
	{
		pos = homePos;
		indicator.SetPos(pos);
		proj.SetPos(pos);
	}
	xRotation = homeXRotation;
	yRotation = homeYRotation;

	const dx::XMFLOAT3 angles = { xRotation,yRotation,0.0f };
	indicator.SetRotation(angles);
	proj.SetRotation(angles);
	proj.Reset(gfx);
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yRotation = wrap_angle(yRotation + dx * rotationSpeed);
	xRotation = std::clamp(xRotation + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
	const dx::XMFLOAT3 angles = { xRotation,yRotation,0.0f };
	indicator.SetRotation(angles);
	proj.SetRotation(angles);

}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	if (!tethered)
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
		proj.SetPos(pos);
	}
}

DirectX::XMFLOAT3 Camera::GetPos() const noexcept
{
	return pos;
}

void Camera::SetPos(const DirectX::XMFLOAT3& pos) noexcept
{
	this->pos = pos;
	indicator.SetPos(pos);
	proj.SetPos(pos);
}

const std::string& Camera::GetName() const noexcept
{
	return name;
}

void Camera::LinkTechniques(RenderGraph::RenderGraph& rg)
{
	indicator.LinkTechniques(rg);
	proj.LinkTechniques(rg);
}

void Camera::Submit(size_t channels) const
{
	if (enableCameraIndicator)
	{
		indicator.Submit(channels);
	}
	if (enableFrustumIndicator)
	{
		proj.Submit(channels);
	}
}
