﻿#include "render/shader.h"

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

void lycoris::render::shader::pixel_shader::set(pixel_shader& shader)
{
    auto& device_context = game::get_game().get_renderer().get_device_context();
    device_context.PSSetShader(shader.pixel_shader_.get(), nullptr, 0);
}

lycoris::render::shader::pixel_shader lycoris::render::shader::pixel_shader::compile(const std::filesystem::path& path, const std::string function_name)
{
    auto shader = pixel_shader();
    auto& renderer = game::get_game().get_renderer();
	
    winrt::com_ptr<ID3DBlob> ps_binary, error_message;
    if (FAILED(D3DCompileFromFile(path.wstring().c_str(), nullptr, nullptr,
        function_name.c_str(), "ps_4_0", flag, 0, ps_binary.put(), error_message.put())))
    {
	    const auto error = static_cast<char*>(error_message->GetBufferPointer());
        throw std::runtime_error("PixelShader: failed to compile: " + std::string(error));
    }
    renderer.get_device().CreatePixelShader(ps_binary->GetBufferPointer(), ps_binary->GetBufferSize(), nullptr, shader.pixel_shader_.put());

    return shader;
}
