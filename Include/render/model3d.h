#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>

#include <DirectXMath.h>

#include "render/renderer.h"
#include "render/texture.h"

namespace lycoris::render::model3d
{
	namespace temp
	{
		struct model_vertex
		{
			std::uint32_t vertex_index;
			std::uint32_t uv_index;
			std::uint32_t normal_index;
		};
	}

	// パーツ1つ1つの頂点情報
	class object_3d
	{
	public:
		object_3d()
		{
			// a
		}
	private:
		std::unique_ptr<ID3D11Buffer> vertex_buffer_, index_buffer_;
	};
	
	// obj のパーツ1つ1つを表す
	class model_part
	{
	public:
		model_part(texture::texture& texture, material& material, object_3d& model)
			: texture_(texture), material_(material), model_(model)
		{
		}

	private:
		texture::texture& texture_;
		material& material_;
		object_3d& model_;
	};

	// 描画用 出来上がりを表す
	class model_3d
	{
	public:
		model_3d(std::vector<model_part>&& parts)
		{
			parts_ = std::move(parts);
		}
		
		~model_3d() = default;
	private:
		std::vector<model_part> parts_;
	};

	class model_loader
	{
	public:
		// Load obj file
		model_3d& LoadObjFile(const std::filesystem::path& path);
		// Load mtl file
		material& LoadMaterialFile(const std::filesystem::path& path);

		model_loader(renderer& renderer) : renderer_(renderer) {}

	private:
		renderer& renderer_;
		std::unordered_map<std::string, std::unique_ptr<model_3d>> parts_;
		std::unordered_map<std::string, std::unique_ptr<material>> materials_;
		texture::texture_loader texture_loader_;
	};
}
