#include "asset/model.h"

#include <vector>

#include "game.h"
#include "utility/file.h"
#include "utility/string.h"

namespace strutil = lycoris::utility::string;

constexpr auto diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

lycoris::render::model3d::model_3d lycoris::asset::load_model(const std::filesystem::path& path)
{
	// filename, used for caching
	// const auto name = path.filename().string();

	auto part = render::model3d::model_part();
	
	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> uv;
	std::vector<render::vertex> vertices;
	std::vector<std::uint32_t> indices;

	std::unordered_map<std::string, render::model3d::model_material> materials;
	
	auto file = utility::file::text_file(path);
	std::vector<std::string> line;

	while (file.next_splitted_line(line))
	{
		if (line[0].starts_with("#") || line.empty()) continue;
		if (line[0] == "v") // vertex
		{
			if (line.size() <= 3)
				throw std::runtime_error("ModelLoader: illegal data detected");
			positions.emplace_back(std::stof(line[1]), std::stof(line[2]), std::stof(line[3]));
		}
		else if (line[0] == "vt") // uv
		{
			if (line.size() <= 2)
				throw std::runtime_error("ModelLoader: illegal data detected");
			uv.emplace_back(std::stof(line[1]), std::stof(line[2]));
		}
		else if (line[0] == "vn") // normals
		{
			if (line.size() <= 3)
				throw std::runtime_error("ModelLoader: illegal data detected");
			normals.emplace_back(std::stof(line[1]), std::stof(line[2]), std::stof(line[3]));
		}
		else if (line[0] == "mtllib") // .mtl file
		{
			auto material = load_material(path);
			materials.insert(material.begin(), material.end());
		}
		else if (line[0] == "usemtl") // specify mtl
		{
			if (!materials.contains(line[1]))
				throw std::runtime_error("ModelLoader: material not found");
			// 読み込んだMaterialをコピーしていれる
			part.materials.push_back(materials[line[1]]);
			if (indices.size() == 0)
			{
				// Materialが1つ目ならばstart_indexは0
				part.materials.back().start_index = 0;
			}
			else
			{
				// Materialが2つ目以降ならstart_indexはindex bufferの要素数-1
				auto& current = part.materials.back();
				current.start_index = indices.size() - 1;
				auto& before = part.materials.at(part.materials.size() - 2);
				// 1つ前のMaterialの頂点数がわかるので計算して代入
				before.indices = before.start_index - current.start_index;
			}
		}
		else if (line[0] == "f") // mesh
		{
			auto vertex_count = line.size() - 1;
			if (!(vertex_count == 3 || vertex_count == 4))
				throw std::runtime_error("ModelLoader: illegal mesh data detected");
			for (std::uint64_t i = 1; i <= vertex_count; i++)
			{
				auto vertex = strutil::split(line[i], "/");
				vertices.emplace_back(
					positions[std::stoi(vertex[0]) - 1ull],
					normals[std::stoi(vertex[1]) - 1ull],
					diffuse,
					uv[std::stoi(vertex[2]) - 1ull]
				);
				indices.emplace_back(vertices.size() - 1);
			}
			// 四角形ポリゴン処理
			if (vertex_count == 4)
			{
				indices.emplace_back(vertices.size() - 4);
				indices.emplace_back(vertices.size() - 2);
			}
		}
	}

	// まだ一番うしろのMaterialが使う頂点数が指定できていないのでここでする
	if (part.materials.size() == 1)
	{
		// Materialが1個しか無いときはindex bufferの数をそのまま入れる
		part.materials.back().indices = indices.size();
	}
	else
	{
		auto& last = part.materials.back();
		last.indices = indices.size() - last.start_index;
	}

	auto& renderer = game::get_game().get_renderer();
	{
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.ByteWidth = sizeof(render::vertex) * vertices.size();
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sub_resource_data{};
		sub_resource_data.pSysMem = vertices.data();

		renderer.get_device().CreateBuffer(&buffer_desc, &sub_resource_data, part.model.put_vertex_buffer());
	}

	{
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.ByteWidth = sizeof(std::uint32_t) * indices.size();
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sub_resource_data{};
		sub_resource_data.pSysMem = indices.data();

		renderer.get_device().CreateBuffer(&buffer_desc, &sub_resource_data, part.model.put_index_buffer());
	}
	return render::model3d::model_3d(std::move(part));
}

std::unordered_map<std::string, lycoris::render::model3d::model_material> lycoris::asset::load_material(const std::filesystem::path& path)
{
	auto file = utility::file::text_file(path);
	std::vector<std::string> line;
	std::unordered_map<std::string, render::model3d::model_material> materials;
	std::string current;
	auto texture_loader = game::get_game().get_texture_loader();
	
	while (file.next_splitted_line(line))
	{
		if (line[0].starts_with("#") || line.empty()) continue;
		if (line[0] == "newmtl")
		{
			materials[line[1]] = render::model3d::model_material();
			current = line[1];
		}
		else if (line[0] == "Ns") // specular shininess
		{
			materials[current].material.shininess = std::stof(line[1]);
		}
		else if (line[0] == "Kd") // diffuse
		{
			auto& diffuse = materials[current].material.diffuse;
			diffuse.x = std::stof(line[1]);
			diffuse.y = std::stof(line[2]);
			diffuse.z = std::stof(line[3]);
		}
		else if (line[0] == "Ks") // specular
		{
			auto& specular = materials[current].material.specular;
			specular.x = std::stof(line[1]);
			specular.y = std::stof(line[2]);
			specular.z = std::stof(line[3]);
		}
		else if (line[0] == "Ka") // ambient
		{
			auto& ambient = materials[current].material.ambient;
			ambient.x = std::stof(line[1]);
			ambient.y = std::stof(line[2]);
			ambient.z = std::stof(line[3]);
		}
		else if (line[0] == "d") // diffuse alpha
		{
			auto& diffuse = materials[current].material.diffuse;
			diffuse.w = std::stof(line[1]);
		}
		else if (line[0] == "map_Kd") // texture
		{
			auto img_path = path;
			img_path.replace_filename(line[1]);
			materials[current].texture = texture_loader.create_texture_from_file(img_path);
		}
	}

	return materials;
}
