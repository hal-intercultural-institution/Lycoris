#include "render/sprite.h"

#include "game.h"

ID3D11Buffer* lycoris::render::sprite::get_vertex_buffer()
{
	return buffer_.get();
}

void lycoris::render::sprite::draw()
{
	auto& renderer = game::get_game().get_renderer();

	renderer.set_depth_enabled(false);
	renderer.set_world_view_projection_2d();

	renderer.set_material(material_);
	
	std::uint32_t stride = sizeof(vertex);
	std::uint32_t offset = 0;
	std::array vertex_buffers = {
		buffer_.get()
	};
	renderer.get_device_context().IASetVertexBuffers(0, vertex_buffers.size(), vertex_buffers.data(), &stride, &offset);
	renderer.get_device_context().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	std::array srvs = {
		texture_.get_shader_resource_view()
	};
	renderer.get_device_context().PSSetShaderResources(0, srvs.size(), srvs.data());
	
	DirectX::XMFLOAT2 uv_offset = { 0.0f, 0.0f };
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
	
	return sprite(std::move(buffer), std::move(texture), material, u_width, v_height);
}

lycoris::render::sprite::sprite(winrt::com_ptr<ID3D11Buffer>&& buffer, texture::texture&& texture, material& material, float u_width, float v_height)
{
	buffer_ = std::move(buffer);
	texture_ = std::move(texture);
	material_ = material;
	u_width_ = u_width;
	v_height_ = v_height;
}
