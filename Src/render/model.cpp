#include "game.h"
#include "render/model3d.h"

void lycoris::render::model3d::model_3d::draw(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale,
	const DirectX::XMFLOAT3& rotation)
{
	auto& renderer = game::get_game().get_renderer();
	auto& device_context = renderer.get_device_context();

	auto world_matrix = XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&scale)));
	world_matrix = XMMatrixMultiply(world_matrix, DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)));
	world_matrix = XMMatrixMultiply(world_matrix, DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&position)));

	DirectX::XMFLOAT4X4 world_matrix_float{};
	XMStoreFloat4x4(&world_matrix_float, world_matrix);

	renderer.set_depth_enabled(true);
	renderer.set_world_matrix(world_matrix_float);
	renderer.set_culling_mode(D3D11_CULL_NONE);
	renderer.set_uv_offset({ 0.0f, 0.0f });

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
		}
	}

}

void lycoris::render::model3d::draw_model(const model_3d& model)
{
	auto& renderer = game::get_game().get_renderer();
	auto& device_context = renderer.get_device_context();

	constexpr auto stride = static_cast<uint32_t>(sizeof(vertex));
	constexpr std::uint32_t offset = 0;
	const std::array vertex_buffers = {
		model.get_vertex_buffer()
	};

	device_context.IASetVertexBuffers(0, static_cast<std::uint32_t>(vertex_buffers.size()), vertex_buffers.data(), &stride, &offset);

	device_context.IASetIndexBuffer(model.get_index_buffer(), DXGI_FORMAT_R32_UINT, 0);

	device_context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	constexpr DirectX::XMFLOAT2 uv_offset = { 0.0f, 0.0f };
	renderer.set_uv_offset(uv_offset);
	
	for (const auto& [texture, material, start_index, indices] : model.get_materials())
	{
		renderer.set_material(material);
		std::array srvs = {
			texture.get_shader_resource_view()
		};
		device_context.PSSetShaderResources(0, static_cast<std::uint32_t>(srvs.size()), srvs.data());
		device_context.DrawIndexed(indices, start_index, 0);
	}
	
}
