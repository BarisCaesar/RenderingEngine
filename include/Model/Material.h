#pragma once
#include "Graphics.h"
#include "BindableCommon.h"
#include <vector>
#include <filesystem>
#include <Technique.h>

class Material
{
public:
	Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd;


	DynamicVertex::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
	
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;
	
	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd;
	
	std::shared_ptr<Bind::IndexBuffer> MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd;
	
	std::vector<Technique> GetTechniques() const noexcept;
private:
	std::string MakeMeshTag(const aiMesh& mesh) const noexcept
	{
		return modelPath + "%" + mesh.mName.C_Str();
	}
private:
	DynamicVertex::VertexLayout vertexLayout;
	std::vector<Technique> techniques;
	std::string modelPath;
	std::string name;
};