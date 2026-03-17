#include "Material.h"
#include "DynamicConstant.h"
#include "ConstantBuffersEx.h"

Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd
	:
	modelPath(path.string())
{
	using namespace Bind;
	const auto rootPath = path.parent_path().string() + "\\";
	{
		aiString tempName;
		material.Get(AI_MATKEY_NAME, tempName);
		name = tempName.C_Str();
	}
	// phong technique
	{
		Technique phong{ "Phong" };
		Step step(0);
		std::string shaderCode = "Phong";
		aiString texFileName;

		// common (pre)
		vertexLayout.Append(DynamicVertex::VertexLayout::Position3D);
		vertexLayout.Append(DynamicVertex::VertexLayout::Normal);
		DynamicConstBuf::RawLayout pscLayout;
		bool hasTexture = false;
		bool hasGlossAlpha = false;

		// diffuse
		{
			bool hasAlpha = false;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Diffuse";
				vertexLayout.Append(DynamicVertex::VertexLayout::Texture2D);
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 0u);
				if (tex->HasAlpha())
				{
					hasAlpha = true;
					shaderCode += "Mask";
				}
				step.AddBindable(std::move(tex));
			}
			else
			{
				pscLayout.Add<DynamicConstBuf::Float3>("materialColor");
			}
			step.AddBindable(Rasterizer::Resolve(gfx, hasAlpha));
		}
		// specular
		{
			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Spec";
				vertexLayout.Append(DynamicVertex::VertexLayout::Texture2D);
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
				hasGlossAlpha = tex->HasAlpha();
				step.AddBindable(std::move(tex));
				pscLayout.Add<DynamicConstBuf::Bool>("useGlossAlpha");
			}
			pscLayout.Add<DynamicConstBuf::Float3>("specularColor");
			pscLayout.Add<DynamicConstBuf::Float>("specularWeight");
			pscLayout.Add<DynamicConstBuf::Float>("specularGloss");
		}
		// normal
		{
			if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Normal";
				vertexLayout.Append(DynamicVertex::VertexLayout::Texture2D);
				vertexLayout.Append(DynamicVertex::VertexLayout::Tangent);
				vertexLayout.Append(DynamicVertex::VertexLayout::Bitangent);
				step.AddBindable(Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2));
				pscLayout.Add<DynamicConstBuf::Bool>("useNormalMap");
				pscLayout.Add<DynamicConstBuf::Float>("normalMapWeight");
			}
		}
		// common (post)
		{
			step.AddBindable(std::make_shared<TransformCBuf>(gfx, 0u));
			step.AddBindable(Blender::Resolve(gfx, false));
			auto pvs = VertexShader::Resolve(gfx, shaderCode + "_VS.cso");
			auto pvsbc = pvs->GetBytecode();
			step.AddBindable(std::move(pvs));
			step.AddBindable(PixelShader::Resolve(gfx, shaderCode + "_PS.cso"));
			step.AddBindable(InputLayout::Resolve(gfx, vertexLayout, pvsbc));
			if (hasTexture)
			{
				step.AddBindable(Bind::Sampler::Resolve(gfx));
			}
			// PS material params (cbuf)
			DynamicConstBuf::Buffer buf{ std::move(pscLayout) };
			if (auto r = buf["materialColor"]; r.Exists())
			{
				aiColor3D color = { 0.45f,0.45f,0.85f };
				material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["useGlossAlpha"].SetIfExists(hasGlossAlpha);
			if (auto r = buf["specularColor"]; r.Exists())
			{
				aiColor3D color = { 0.18f,0.18f,0.18f };
				material.Get(AI_MATKEY_COLOR_SPECULAR, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["specularWeight"].SetIfExists(1.0f);
			if (auto r = buf["specularGloss"]; r.Exists())
			{
				float gloss = 8.0f;
				material.Get(AI_MATKEY_SHININESS, gloss);
				r = gloss;
			}
			buf["useNormalMap"].SetIfExists(true);
			buf["normalMapWeight"].SetIfExists(1.0f);
			step.AddBindable(std::make_unique<Bind::CachingPixelConstantBufferEX>(gfx, std::move(buf), 1u));
		}
		phong.AddStep(std::move(step));
		techniques.push_back(std::move(phong));
	}
	// outline technique
	{
		Technique outline("Outline", false);
		{
			Step mask(1);
			
			auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
			auto pvsbc = pvs->GetBytecode();
			mask.AddBindable(std::move(pvs));

			mask.AddBindable(InputLayout::Resolve(gfx, vertexLayout, pvsbc));

			mask.AddBindable(std::make_shared<TransformCBuf>(gfx));

			outline.AddStep(std::move(mask));
		}
		{
			Step draw(2);

			auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
			auto pvsbc = pvs->GetBytecode();
			
			draw.AddBindable(std::move(pvs));

			draw.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

			DynamicConstBuf::RawLayout lay;
			lay.Add<DynamicConstBuf::Float3>("materialColor");
			auto buf = DynamicConstBuf::Buffer(std::move(lay));
			buf["materialColor"] = DirectX::XMFLOAT3{ 1.f, 0.4f, 0.4f };
			draw.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEX>(gfx, buf, 1u));

			draw.AddBindable(InputLayout::Resolve(gfx, vertexLayout, pvsbc));

			class TransformCBufScaling : public TransformCBuf
			{
			public:
				TransformCBufScaling(Graphics& gfx, float scale = 1.04)
					:
					TransformCBuf(gfx),
					buf(MakeLayout())
				{
					buf["scale"] = scale;
				}
				void Accept(TechniqueProbe& probe) override
				{
					probe.VisitBuffer(buf);
				}
				void Bind(Graphics& gfx) noexcept override
				{
					const float scale = buf["scale"];
					const auto scaleMatrix = DirectX::XMMatrixScaling(scale, scale, scale);
					auto xf = GetTransforms(gfx);
					xf.modelView = xf.modelView * scaleMatrix;
					xf.modelViewProj = xf.modelViewProj * scaleMatrix;
					UpdateBindImpl(gfx, xf);
				}
				std::unique_ptr<CloningBindable> Clone() const noexcept override
				{
					return std::make_unique<TransformCBufScaling>(*this);
				}
			private:
				static DynamicConstBuf::RawLayout MakeLayout()
				{
					DynamicConstBuf::RawLayout layout;
					layout.Add<DynamicConstBuf::Float>("scale");
					return layout;
				}
			private:
				DynamicConstBuf::Buffer buf;
			};
			draw.AddBindable(std::make_shared<TransformCBufScaling>(gfx));

			outline.AddStep(std::move(draw));
		}
		techniques.push_back(std::move(outline));
	}
}

DynamicVertex::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
{
	return { vertexLayout, mesh };
}

std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept
{
	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	DynamicVertex::VertexBuffer buf{ vertexLayout };
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	return indices;
}
std::shared_ptr<Bind::VertexBuffer> Material::MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale) const noxnd
{
	auto vertices = ExtractVertices(mesh);
	if (scale != 1.f)
	{
		for (auto i = 0u; i < vertices.Size(); i++)
		{
			DirectX::XMFLOAT3& pos = vertices[i].Attr<DynamicVertex::VertexLayout::ElementType::Position3D>();
			pos.x *= scale;
			pos.y *= scale;
			pos.z *= scale;
		}
	}
	return Bind::VertexBuffer::Resolve(gfx, MakeMeshTag(mesh), std::move(vertices));
}
std::shared_ptr<Bind::IndexBuffer>  Material::MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd
{
	return Bind::IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
}
std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept
{
	return modelPath + "%" + mesh.mName.C_Str();
}
std::vector<Technique> Material::GetTechniques() const noexcept
{
	return techniques;
}



