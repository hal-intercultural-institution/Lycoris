#include "game.h"
#include "render/model3d.h"

void lycoris::render::model3d::model_3d::draw(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale,
	const DirectX::XMFLOAT3& rotation)
{
	auto& game = game::get_game();
	auto& renderer = game.get_renderer();
	auto& device_context = renderer.get_device_context();

	auto world_matrix = XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&scale)));
	world_matrix = XMMatrixMultiply(world_matrix, DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)));
	world_matrix = XMMatrixMultiply(world_matrix, DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&position)));

	DirectX::XMFLOAT4X4 world_matrix_float{};
	XMStoreFloat4x4(&world_matrix_float, world_matrix);

	renderer.set_depth_enabled(true);
	renderer.set_world_matrix(world_matrix_float);
	renderer.set_culling_mode(culling_mode::none);
	renderer.set_blend_state(blend_state::none);
	renderer.set_uv_offset({ 0.0f, 0.0f });
	renderer.set_vertex_shader(shader::vertex::normal);

	// for (parts) ...
	{
		constexpr auto stride = static_cast<uint32_t>(sizeof(vertex));
		constexpr std::uint32_t offset = 0;
		const std::array vertex_buffers = {
			vertex_buffer_.get()
		};

		device_context.IASetVertexBuffers(0, static_cast<std::uint32_t>(vertex_buffers.size()), vertex_buffers.data(), &stride, &offset);

		device_context.IASetIndexBuffer(index_buffer_.get(), DXGI_FORMAT_R32_UINT, 0);

		device_context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (auto& [texture, material, start_index, indices] : materials_)
		{
			renderer.set_material(material);
			const std::array srvs = {
				texture.get_shader_resource_view()
			};
			device_context.PSSetShaderResources(0, static_cast<std::uint32_t>(srvs.size()), srvs.data());
			device_context.DrawIndexed(indices, start_index, 0);
			game.increment_draw_call_count();
		}
	}

}

void lycoris::render::model3d::animated_model::draw(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale,
	const DirectX::XMFLOAT3& rotation, const animation::animator& animator)
{
	auto& game = game::get_game();
	auto& renderer = game.get_renderer();
	auto& device_context = renderer.get_device_context();

	auto world_matrix = XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&scale)));
	world_matrix = XMMatrixMultiply(world_matrix, DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)));
	world_matrix = XMMatrixMultiply(world_matrix, DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&position)));

	DirectX::XMFLOAT4X4 world_matrix_float{};
	XMStoreFloat4x4(&world_matrix_float, world_matrix);

	renderer.set_depth_enabled(true);
	renderer.set_world_matrix(world_matrix_float);
	renderer.set_culling_mode(culling_mode::none);
	renderer.set_uv_offset({ 0.0f, 0.0f });
	renderer.set_animation(animator);
	renderer.set_vertex_shader(shader::vertex::animated);

	constexpr auto stride = static_cast<uint32_t>(sizeof(animated_vertex));
	constexpr std::uint32_t offset = 0;
	const std::array vertex_buffers = {
		vertex_buffer_.get()
	};

	device_context.IASetVertexBuffers(0, static_cast<std::uint32_t>(vertex_buffers.size()), vertex_buffers.data(), &stride, &offset);

	device_context.IASetIndexBuffer(index_buffer_.get(), DXGI_FORMAT_R32_UINT, 0);

	device_context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& [texture, material, start_index, indices] : materials_)
	{
		renderer.set_material(material);
		const std::array srvs = {
			texture.get_shader_resource_view()
		};
		device_context.PSSetShaderResources(0, static_cast<std::uint32_t>(srvs.size()), srvs.data());
		device_context.DrawIndexed(indices, start_index, 0);
		game.increment_draw_call_count();
	}
}
