#pragma once

#include <vector>
#include <cstdint>

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

	struct model_material
	{
		texture::texture texture;
		material material;
		std::uint32_t start_index;
		std::uint32_t indices;
	};
	
	// Objファイル1個
	class object_3d
	{
	public:
		ID3D11Buffer** put_vertex_buffer()
		{
			return vertex_buffer_.put();
		}
		ID3D11Buffer** put_index_buffer()
		{
			return index_buffer_.put();
		}
		ID3D11Buffer* get_vertex_buffer()
		{
			return vertex_buffer_.get();
		}
		ID3D11Buffer* get_index_buffer()
		{
			return index_buffer_.get();
		}
	
	private:
		winrt::com_ptr<ID3D11Buffer> vertex_buffer_, index_buffer_;
		
	};
	
	// objとmtlを合体したやつ
	// (今後アニメーションもここに入れるかも)
	class model_part
	{
	public:
		model_part() = default;
		model_part(const model_part&) = delete;
		model_part(model_part&&) = default;
		~model_part() = default;

		model_part& operator=(const model_part&) = delete;
		model_part& operator=(model_part&&) = default;

		object_3d model{};
		std::vector<model_material> materials{};
		//std::uint32_t parent_index_;
	};

	// 描画用 出来上がりを表す
	class model_3d
	{
	public:
		//model_3d(std::vector<model_part>&& parts, std::vector<material>&& materials)
		//	: parts_(std::move(parts)), materials_(std::move(materials))
		//{
		//}
		model_3d() = default;
		explicit model_3d(model_part&& part)
		{
			part_ = std::move(part);
		}
		model_3d(const model_3d&) = delete;
		model_3d(model_3d&&) = default;
		~model_3d() = default;
		
		model_3d& operator=(const model_3d&) = delete;
		model_3d& operator=(model_3d&&) = default;

		model_part& get_parts()
		{
			return part_;
		}

		void draw(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation);
	
	private:
		//std::vector<model_part> parts_;
		//std::vector<material> materials_;
		model_part part_;
	};

	void draw_model(model_3d& model);
	
}
