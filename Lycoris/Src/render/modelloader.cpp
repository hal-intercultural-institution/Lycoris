#include "render/model3d.h"

#include <stdexcept>

#include "utility/file.h"
#include "utility/string.h"

//namespace strutil = lycoris::utility::string;
//
//lycoris::render::model3d::model_3d& lycoris::render::model3d::model_loader::LoadObjFile(const std::filesystem::path& path)
//{
//	auto result = std::make_unique<model_3d>();
//	const auto name = path.filename().string();
//	if (models_.contains(name)) return *models_[name];
//
//	// temporary objects
//	std::vector<DirectX::XMFLOAT3> positions;
//	std::vector<DirectX::XMFLOAT3> normals;
//	std::vector<DirectX::XMFLOAT2> uv;
//	std::vector<vertex> vertices;
//	
//	std::vector<std::string> file;
//	try
//	{
//		file = utility::file::load_text_file(path);
//	}
//	catch (const std::runtime_error& e)
//	{
//		throw std::runtime_error(std::string("ModelLoader: failed to load obj: ") + e.what());
//	}
//
//	for (std::uint64_t line_index = 0ull; std::string& line : file)
//	{
//		++line_index; // èXÇ¢
//		if (line.starts_with("#")) continue;
//
//		if (std::vector<std::string> data = strutil::split(line, std::string(" ")); data[0] == "mtlib") // Load Material
//		{
//			auto mtlPath = path;
//			mtlPath.replace_filename(data[1]);
//			LoadMaterialFile(mtlPath);
//		}
//		else if (data[0] == "v") // Vertex
//		{
//			if (!data.size() < 4) throw std::runtime_error(
//				"ModelLoader: failed to load obj: invalid data in file " + name + "at " + std::to_string(line_index));
//			positions.emplace_back(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));
//		}
//		else if (data[0] == "vt") // Texture
//		{
//			if (!data.size() < 3) throw std::runtime_error(
//				"ModelLoader: failed to load obj: invalid data in file " + name + "at " + std::to_string(line_index));
//			uv.emplace_back(std::stof(data[1]), std::stof(data[2]));
//		}
//		else if (data[0] == "vn") // Normal
//		{
//			if (!data.size() < 4) throw std::runtime_error(
//				"ModelLoader: failed to load obj: invalid data in file " + name + "at " + std::to_string(line_index));
//			normals.emplace_back(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));
//		}
//		else if (data[0] == "f") // Polygon
//		{
//			if (!data.size() < 4) throw std::runtime_error(
//				"ModelLoader: failed to load obj: invalid data in file " + name + "at " + std::to_string(line_index));
//
//			auto v1 = strutil::split(data[1], "/");
//			auto v2 = strutil::split(data[2], "/");
//			auto v3 = strutil::split(data[3], "/");
//			groups.back().mesh.emplace_back(
//				std::stoi(v1[0]), std::stoi(v1[1]), std::stoi(v1[2]),
//				std::stoi(v2[0]), std::stoi(v2[1]), std::stoi(v2[2]),
//				std::stoi(v3[0]), std::stoi(v3[1]), std::stoi(v3[2])
//			);
//		}
//	}
//}
//
//lycoris::render::material& lycoris::render::model3d::model_loader::LoadMaterialFile(const std::filesystem::path& path)
//{
//	std::vector<std::string> file;
//	try
//	{
//		file = utility::file::load_text_file(path);
//	}
//	catch (const std::runtime_error& e)
//	{
//		throw std::runtime_error(std::string("ModelLoader: failed to load material: ") + e.what());
//	}
//
//	std::uint64_t lineIndex = 0ull;
//	std::string currentMtl = "";
//
//	for (std::string& line : file)
//	{
//		++lineIndex;
//		if (line.starts_with("#")) continue;
//		std::vector<std::string> data = strutil::split(line, std::string(" "));
//		if (data[0] == "newmtl") // Material Begin
//		{
//			materials_[data[1]] = std::make_unique<material>();
//			currentMtl = data[1];
//		}
//		else if (data[0] == "Ka") // Ambient
//		{
//			materials_[currentMtl]->ambient = { std::stof(data[1]), std::stof(data[2]), std::stof(data[3]), 1.0f };
//		}
//		else if (data[0] == "Kd") // Diffuse
//		{
//			materials_[currentMtl]->diffuse = { std::stof(data[1]), std::stof(data[2]), std::stof(data[3]), 1.0f };
//		}
//		else if (data[0] == "Ks") // Specular
//		{
//			materials_[currentMtl]->specular = { std::stof(data[1]), std::stof(data[2]), std::stof(data[3]), 1.0f };
//		}
//		else if (data[0] == "Ns") // Specular Shininess
//		{
//			materials_[currentMtl]->shininess = std::stof(data[1]);
//		}
//		else if (data[0] == "d") // Diffuse Alpha
//		{
//			materials_[currentMtl]->diffuse.w = std::stof(data[1]);
//		}
//		else if (data[0] == "map_Kd") // Texture
//		{
//			// load texture
//		}
//	}
//}
