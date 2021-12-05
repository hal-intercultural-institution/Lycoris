#include "render/shader.h"

#include <d3dcompiler.h>

#include "render/renderer.h"
#include "game.h"

namespace
{
#ifdef _DEBUG
    constexpr std::uint32_t flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    constexpr std::uint32_t flag = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

}

lycoris::render::shader::vertex_shader::vertex_shader(winrt::com_ptr<ID3D11VertexShader>&& vertex_shader,
	winrt::com_ptr<ID3D11InputLayout>&& input_layout)
{
    vertex_shader_ = std::move(vertex_shader);
    input_layout_ = std::move(input_layout);
}

ID3D11VertexShader& lycoris::render::shader::vertex_shader::get_shader() const noexcept
{
    assert(vertex_shader_);
    return *vertex_shader_;
}

ID3D11InputLayout& lycoris::render::shader::vertex_shader::get_input_layout() const noexcept
{
    assert(input_layout_);
    return *input_layout_;
}

lycoris::render::shader::pixel_shader::pixel_shader(winrt::com_ptr<ID3D11VertexShader>&& pixel_shader)
{
    pixel_shader_ = std::move(pixel_shader);
}

ID3D11PixelShader& lycoris::render::shader::pixel_shader::get_shader() const
{
    assert(pixel_shader_);
    return *pixel_shader_;
}
