#pragma once

#include <unordered_map>

#include "render/model3d.h"

namespace lycoris::asset
{
	namespace temp
	{
		struct vert
		{
			DirectX::XMFLOAT3 v;
			std::uint32_t index;
		};
	}

	render::model3d::model_3d load_model(const std::filesystem::path& path);
	render::model3d::animated_model load_animated_model(const std::filesystem::path& obj_path, const std::filesystem::path& ind_path);

	std::unordered_map<std::string, std::uint32_t> load_object_indices(const std::filesystem::path& path);
	std::unordered_map<std::string, render::model3d::model_material> load_material(const std::filesystem::path& path);
}