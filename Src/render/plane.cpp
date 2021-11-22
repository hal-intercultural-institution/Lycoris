#include "render/plane.h"

#include "game.h"

ID3D11Buffer const* lycoris::render::plane::get_vertex_buffer() const
{
	return buffer_.get();
}

lycoris::render::material& lycoris::render::plane::get_material()
{
	return material_;
}

lycoris::render::blend_state lycoris::render::plane::get_blend_state() const
{
	return blend_state_;
}

void lycoris::render::plane::set_billboard(const bool billboard)
{
	billboard_ = billboard;
}

void lycoris::render::plane::set_blend_state(const blend_state state)
{
	blend_state_ = state;
}

void lycoris::render::plane::draw(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation)
{
	constexpr DirectX::XMFLOAT3 compensation = { 0.0f, DirectX::XM_PI, 0.0f};
	auto& renderer = game::get_game().get_renderer();
	auto& device_context = renderer.get_device_context();

	auto world_matrix = XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&scale)));
	world_matrix = XMMatrixMultiply(world_matrix, 
		billboard_ ? XMLoadFloat4x4(&renderer.get_camera().get_inverted_view_matrix()) :
		DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMVectorAdd(XMLoadFloat3(&rotation), XMLoadFloat3(&compensation))));
	world_matrix = XMMatrixMultiply(world_matrix, DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&position)));

	DirectX::XMFLOAT4X4 world_matrix_float{};
	XMStoreFloat4x4(&world_matrix_float, world_matrix);

	renderer.set_depth_enabled(true);
	renderer.set_world_matrix(world_matrix_float);
	renderer.set_culling_mode(D3D11_CULL_BACK);
	renderer.set_blend_state(blend_state_);
	renderer.set_uv_offset({ 0.0f, 0.0f });

	constexpr auto stride = static_cast<uint32_t>(sizeof(vertex));
	constexpr std::uint32_t offset = 0;
	const std::array vertex_buffers = {
		buffer_.get()
	};
	device_context.IASetVertexBuffers(0, static_cast<std::uint32_t>(vertex_buffers.size()), vertex_buffers.data(), &stride, &offset);

	device_context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	renderer.set_material(material_);
	const std::array srvs = {
		texture_.get_shader_resource_view()
	};
	device_context.PSSetShaderResources(0, static_cast<std::uint32_t>(srvs.size()), srvs.data());
	device_context.Draw(4, 0);
}

lycoris::render::plane lycoris::render::plane::create(float width, float height, texture::texture&& texture)
{
	const auto& renderer = game::get_game().get_renderer();

	material material{};
	material.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	constexpr auto vertex_count = 4;

	winrt::com_ptr<ID3D11Buffer> buffer = nullptr;
	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(vertex) * vertex_count;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;

	const std::array<vertex, vertex_count> vertices = {
		{
			{ { width * -0.5f, height * 0.5f, 0.0f }, { .0f, 1.0f, .0f},  { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
			{ { width * 0.5f, height * 0.5f, 0.0f }, { .0f, 1.0f, .0f},  { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
			{ { width * -0.5f, height * -0.5f, 0.0f }, { .0f, 1.0f, .0f},  { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
			{ { width * 0.5f, height * -0.5f, 0.0f }, { .0f, 1.0f, .0f},  { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
		}
	};

	D3D11_SUBRESOURCE_DATA sub_resource_data{};
	sub_resource_data.pSysMem = vertices.data();

	renderer.get_device().CreateBuffer(&desc, &sub_resource_data, buffer.put());

	return plane(std::move(buffer), std::move(texture), material, width, height);
}

lycoris::render::plane lycoris::render::plane::create(const float width, const float height, const std::filesystem::path& path)
{
	return create(width, height, game::get_game().get_texture_loader().create_texture_from_file(path));
}

lycoris::render::plane::plane(winrt::com_ptr<ID3D11Buffer>&& buffer, texture::texture&& texture, const material& material, float width, float height)
{
	buffer_ = std::move(buffer);
	texture_ = std::move(texture);
	material_ = material;
}
