#pragma once

#include "render/renderer.h"
#include "render/texture.h"
#include "render/alignment.h"

namespace lycoris::render
{
	class sprite
	{
	public:
		sprite() = default;
		sprite(const sprite&) = delete;
		sprite(sprite&&) = default;

		sprite& operator=(const sprite&) = delete;
		sprite& operator=(sprite&&) = default;

		~sprite() = default;
		
		ID3D11Buffer* get_vertex_buffer();
		DirectX::XMFLOAT2& get_position();
		std::int32_t& get_u_index();
		std::int32_t& get_v_index();
		material& get_material();
		
		void set_position(const DirectX::XMFLOAT2& position);
		void set_u_index(std::int32_t index);
		void set_v_index(std::int32_t index);
		void set_material(const material& material);
		void set_horizontal_alignment(horizontal_alignment alignment) noexcept;
		void set_vertical_alignment(vertical_alignment alignment) noexcept;
		
		void draw();

		static sprite create(float width, float height, std::uint32_t u, std::uint32_t v, texture::texture&& texture);
		static sprite create(float width, float height, std::uint32_t u, std::uint32_t v, const std::filesystem::path& path);
	
	private:
		sprite(winrt::com_ptr<ID3D11Buffer>&& buffer, texture::texture&& texture, const material& material, float width, float height, float u_width, float v_height);
		winrt::com_ptr<ID3D11Buffer> buffer_;
		texture::texture texture_;
		material  material_{};
		DirectX::XMFLOAT2 position_{};
		horizontal_alignment horizontal_alignment_ = horizontal_alignment::left;
		vertical_alignment vertical_alignment_ = vertical_alignment::top;
		float u_width_ = 1.0f, v_height_ = 1.0f;
		float width_ = 0.0f, height_ = 0.0f;
		std::int32_t u_index_ = 0, v_index_ = 0;
	};
}
