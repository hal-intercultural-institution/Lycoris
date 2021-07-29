#pragma once

#include <unordered_map>

#include "render/model3d.h"

namespace lycoris::asset
{
	render::model3d::model_3d load_model(const std::filesystem::path& path);
	//render::model3d::object_3d load_object(const std::filesystem::path& path);
	std::unordered_map<std::string, render::model3d::model_material> load_material(const std::filesystem::path& path);
}