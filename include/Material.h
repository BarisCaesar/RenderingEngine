#pragma once
#include <assimp/scene.h>
#include "BindableCommon.h"
#include <vector>
#include <filesystem>
#include <Technique.h>

class Material
{
public:
	Material(Graphics& gfx, const aiMaterial* pMaterial, const std::filesystem::path& path) noxnd
	{

	}
	DynamicVertex::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept
	{
		using Type = DynamicVertex::VertexLayout::ElementType;
		DynamicVertex::VertexBuffer buf{ vertexLayout };
		buf.Resize(mesh.mNumVertices);
		if (vertexLayout.Has<Type::Position3D>())
		{
			for (int i = 0; i < mesh.mNumVertices; i++)
			{
				//buf[i].
			}
		}
	}
	std::vector<Technique> GetTechniques() const noexcept
	{
		return techniques;
	}
private:
	DynamicVertex::VertexLayout vertexLayout;
	std::vector<Technique> techniques;
};