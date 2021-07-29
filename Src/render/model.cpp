#include "game.h"
#include "render/model3d.h"

void lycoris::render::model3d::draw_model(model_3d& model)
{
	auto& renderer = game::get_game().get_renderer();
	auto& device_context = renderer.get_device_context();
	auto& parts = model.get_parts();
	// for (parts) ...
	{
		auto stride = static_cast<uint32_t>(sizeof(vertex));
		std::uint32_t offset = 0;
		std::array vertex_buffers = {
			parts.model.get_vertex_buffer()
		};
		device_context.IASetVertexBuffers(0, vertex_buffers.size(), vertex_buffers.data(), &stride, &offset);

		device_context.IASetIndexBuffer(parts.model.get_index_buffer(), DXGI_FORMAT_R32_UINT, 0);

		device_context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		for (auto& [texture, material, start_index, indices] : parts.materials)
		{
			renderer.set_material(material);
			std::array srvs = {
				texture.get_shader_resource_view()
			};
			device_context.PSSetShaderResources(0, srvs.size(), srvs.data());
			device_context.DrawIndexed(indices, start_index, 0);
		}
	}
}
