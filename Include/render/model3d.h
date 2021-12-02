#pragma once

#include <vector>
#include <cstdint>

#include "animation.h"
#include "render/renderer.h"
#include "render/texture.h"

namespace lycoris::render::model3d
{

	struct model_material
	{
		texture::texture texture;
		material material;
		std::uint32_t start_index;
		std::uint32_t indices;
	};

	class model_3d
	{
	public:
		model_3d() = default;
		explicit model_3d(winrt::com_ptr<ID3D11Buffer>&& vertex_buffer, winrt::com_ptr<ID3D11Buffer>&& index_buffer, std::vector<model_material>&& materials)
		{
			vertex_buffer_ = std::move(vertex_buffer);
			index_buffer_ = std::move(index_buffer);
			materials_ = std::move(materials);
		}
		model_3d(const model_3d&) = delete;
		model_3d(model_3d&&) = default;
		~model_3d() = default;
		
		model_3d& operator=(const model_3d&) = delete;
		model_3d& operator=(model_3d&&) = default;

		ID3D11Buffer** put_vertex_buffer()
		{
			return vertex_buffer_.put();
		}
		ID3D11Buffer** put_index_buffer()
		{
			return index_buffer_.put();
		}
		ID3D11Buffer* get_vertex_buffer() const
		{
			return vertex_buffer_.get();
		}
		ID3D11Buffer* get_index_buffer() const
		{
			return index_buffer_.get();
		}
		const std::vector<model_material>& get_materials() const
		{
			return materials_;
		}

		void draw(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation);

	private:
		winrt::com_ptr<ID3D11Buffer> vertex_buffer_, index_buffer_;
		std::vector<model_material> materials_{};
	};

	class animated_model
	{
	public:
		animated_model() = default;
		explicit animated_model(winrt::com_ptr<ID3D11Buffer> && vertex_buffer, winrt::com_ptr<ID3D11Buffer> && index_buffer, std::vector<model_material> && materials)
		{
			vertex_buffer_ = std::move(vertex_buffer);
			index_buffer_ = std::move(index_buffer);
			materials_ = std::move(materials);
		}
		animated_model(const animated_model&) = delete;
		animated_model(animated_model&&) = default;
		~animated_model() = default;

		animated_model& operator=(const animated_model&) = delete;
		animated_model& operator=(animated_model&&) = default;

		ID3D11Buffer* get_vertex_buffer() const
		{
			return vertex_buffer_.get();
		}
		ID3D11Buffer* get_index_buffer() const
		{
			return index_buffer_.get();
		}
		const std::vector<model_material>& get_materials() const
		{
			return materials_;
		}

		void draw(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation, const animation::animator& animator);

	private:
		winrt::com_ptr<ID3D11Buffer> vertex_buffer_, index_buffer_;
		std::vector<model_material> materials_{};
	};

	void draw_model(const model_3d& model);
	
}
