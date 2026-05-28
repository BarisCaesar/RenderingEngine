#pragma once
#include <vector>
#include <memory>

class Camera;
class Graphics;
namespace RenderGraph
{
	class RenderGraph;
}

class CameraContainer
{
public:
	void SpawnWindow();
	void Bind(Graphics& gfx);
	void AddCamera(std::unique_ptr<Camera> pCam);
	Camera& GetCamera();
	~CameraContainer();
	void LinkTechniques(RenderGraph::RenderGraph& rg);
	void Submit() const;
private:
	std::vector<std::unique_ptr<Camera>> cameras;
	int selected = 0;
};