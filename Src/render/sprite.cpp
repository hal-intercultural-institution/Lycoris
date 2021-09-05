#include "render/sprite.h"

#include "utility/cast.h"
#include "game.h"

using lycoris::utility::scast::uint32_of;

ID3D11Buffer* lycoris::render::sprite::get_vertex_buffer()
{
	return buffer_.get();
}

DirectX::XMFLOAT2& lycoris::render::sprite::get_position()
{
	return position_;
}

std::int32_t& lycoris::render::sprite::get_u_index()
{
	return u_index_;
}

std::int32_t& lycoris::render::sprite::get_v_index()
{
	return v_index_;
}

lycoris::render::material& lycoris::render::sprite::get_material()
{
	return material_;
}

void lycoris::render::sprite::set_position(const DirectX::XMFLOAT2& position)
{
	position_ = position;
}

void lycoris::render::sprite::set_u_index(const std::int32_t index)
{
	u_index_ = index;
}

void lycoris::render::sprite::set_v_index(const std::int32_t index)
{
	v_index_ = index;
}

void lycoris::render::sprite::set_material(const material& material)
{
	material_ = material;
}

void lycoris::render::sprite::set_horizontal_alignment(const horizontal_alignment alignment) noexcept
{
	horizontal_alignment_ = alignment;
}

void lycoris::render::sprite::set_vertical_alignment(const vertical_alignment alignment) noexcept
{
	vertical_alignment_ = alignment;
}

void lycoris::render::sprite::draw()
{
	if (u_index_ < 0) u_index_ = 0;
	if (v_index_ < 0) v_index_ = 0;
	
	auto& renderer = game::get_game().get_renderer();

	renderer.set_depth_enabled(false);
	renderer.set_culling_mode(D3D11_CULL_BACK);

	renderer.set_view_projection_2d();

	float x_pos = position_.x, y_pos = position_.y;

	switch (horizontal_alignment_)
	{
	case horizontal_alignment::right:
		x_pos -= width_;
		break;
	case horizontal_alignment::center:
		x_pos -= width_* 0.5f;
		break;
	default:
		break;
	}

	switch (vertical_alignment_)
	{
	case vertical_alignment::bottom:
		y_pos -= height_;
		break;
	case vertical_alignment::middle:
		y_pos -= height_ * 0.5f;
		break;
	default:
		break;
	}

	world_matrix = XMMatrixMultiply(world_matrix, DirectX::XMMatrixTranslation(x_pos, y_pos, 0.0f));

	DirectX::XMFLOAT4X4 world_matrix_float{};
	XMStoreFloat4x4(&world_matrix_float, world_matrix);
	renderer.set_world_matrix(world_matrix_float);

	renderer.set_material(material_);

	std::uint32_t stride = sizeof(vertex);
	std::uint32_t offset = 0;
	std::array vertex_buffers = {
		buffer_.get()
	};
	renderer.get_device_context().IASetVertexBuffers(0, uint32_of(vertex_buffers.size()), vertex_buffers.data(), &stride, &offset);
	renderer.get_device_context().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	std::array srvs = {
		texture_.get_shader_resource_view()
	};
	renderer.get_device_context().PSSetShaderResources(0, uint32_of(srvs.size()), srvs.data());

	DirectX::XMFLOAT2 uv_offset = { u_width_ * u_index_, v_height_ * v_index_ };
	renderer.set_uv_offset(uv_offset);

	renderer.get_device_context().Draw(4, 0);
}

lycoris::render::sprite lycoris::render::sprite::create(const float width, const float height, const std::uint32_t u, const std::uint32_t v, texture::texture&& texture)
{
	auto& renderer = game::get_game().get_renderer();

	material material{};
	material.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	constexpr auto vertex_count = 4;
	
	winrt::com_ptr<ID3D11Buffer> buffer = nullptr;
	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(vertex) * vertex_count;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;

	const auto u_width = 1.0f / u;
	const auto v_height = 1.0f / v;
	
	std::array<vertex, vertex_count> vertices = {
		{
			{ { 0.0f, 0.0f, 0.0f }, {},  { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
			{ { width, 0.0f, 0.0f }, {},  { 1.0f, 1.0f, 1.0f, 1.0f }, { u_width, 0.0f } },
			{ { 0.0f, height, 0.0f }, {},  { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, v_height } },
			{ { width, height, 0.0f }, {},  { 1.0f, 1.0f, 1.0f, 1.0f }, { u_width, v_height } },
		}
	};

	D3D11_SUBRESOURCE_DATA sub_resource_data{};
	sub_resource_data.pSysMem = vertices.data();

	renderer.get_device().CreateBuffer(&desc, &sub_resource_data, buffer.put());
	
	return sprite(std::move(buffer), std::move(texture), material, width, height, u_width, v_height);
}

lycoris::render::sprite lycoris::render::sprite::create(const float width, const float height, const std::uint32_t u, const std::uint32_t v,
                                                        const std::filesystem::path& path)
{
	return create(width, height, u, v, game::get_game().get_texture_loader().create_texture_from_file(path));
}

lycoris::render::sprite::sprite(winrt::com_ptr<ID3D11Buffer>&& buffer, texture::texture&& texture,
	const material& material, const float width, const float height, const float u_width, const float v_height)
{
	buffer_ = std::move(buffer);
	texture_ = std::move(texture);
	material_ = material;
	u_width_ = u_width;
	v_height_ = v_height;
	width_ = width;
	height_ = height;
	u_index_ = v_index_ = 0;
}
