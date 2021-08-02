#pragma once

#include <filesystem>

#include <d3d11.h>
#include <winrt/base.h>

namespace lycoris::render::shader
{
    class vertex_shader
    {
    public:
        static void set(vertex_shader& shader);
        static vertex_shader compile(std::filesystem::path path, std::string function_name,
            std::initializer_list<D3D11_INPUT_ELEMENT_DESC>& input_layout);
    private:
        winrt::com_ptr<ID3D11VertexShader> vertex_shader_;
        winrt::com_ptr<ID3D11InputLayout> input_layout_;
    };

	class pixel_shader
	{
	public:
        static void set(pixel_shader& shader);
        static pixel_shader compile(std::filesystem::path path, std::string function_name);
	private:
        winrt::com_ptr<ID3D11PixelShader> pixel_shader_;
	};

}
