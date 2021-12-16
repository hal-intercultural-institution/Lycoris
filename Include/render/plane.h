#pragma once

#include "render/renderer.h"
#include "render/texture.h"

namespace lycoris::render
{
	class plane
	{
	public:
		plane() = default;
		plane(const plane&) = delete;
		plane(plane&&) = default;

		plane& operator=(const plane&) = delete;
		plane& operator=(plane&&) = default;

		~plane() = default;

		ID3D11Buffer const* get_vertex_buffer() const;
		material& get_material();
		blend_state get_blend_state() const;

		void set_billboard(bool billboard);
		void set_blend_state(blend_state state);
		void set_alpha_test_state(bool enabled);
		void set_culling_mode(culling_mode mode) noexcept;

		void draw(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation);

		static plane create(float width, float height, texture::texture&& texture);
		static plane create(float width, float height, const std::filesystem::path& path);

	private:
		plane(winrt::com_ptr<ID3D11Buffer>&& buffer, texture::texture&& texture, const material& material, float width, float height);
		winrt::com_ptr<ID3D11Buffer> buffer_;
		texture::texture texture_;
		material  material_{};
		blend_state blend_state_ = blend_state::none;
		culling_mode culling_mode_ = culling_mode::back;
		bool billboard_ = false;
		bool alpha_test_ = false;
	};
}