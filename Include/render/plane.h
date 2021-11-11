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

		DirectX::XMFLOAT3& get_position();
		DirectX::XMFLOAT3& get_rotation();
		DirectX::XMFLOAT3& get_scale();

		void set_position(const DirectX::XMFLOAT3& position);
		void set_rotation(const DirectX::XMFLOAT3& rotation);
		void set_scale(const DirectX::XMFLOAT3& scale);
		void set_billboard(bool billboard);

		void draw(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation);

		static plane create(float width, float height, texture::texture&& texture);
		static plane create(float width, float height, const std::filesystem::path& path);

	private:
		plane(winrt::com_ptr<ID3D11Buffer>&& buffer, texture::texture&& texture, const material& material, float width, float height);
		winrt::com_ptr<ID3D11Buffer> buffer_;
		texture::texture texture_;
		material  material_{};
		bool billboard_ = false;
	};
}