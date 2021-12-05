#pragma once

#include <filesystem>

#include <d3d11.h>
#include <winrt/base.h>

namespace lycoris::render::shader
{
    enum class vertex
    {
	    normal,
        animated,
        max
    };

	class vertex_shader
    {
    public:
        vertex_shader() = default;
        vertex_shader(winrt::com_ptr<ID3D11VertexShader>&& vertex_shader, winrt::com_ptr<ID3D11InputLayout>&& input_layout);
        vertex_shader(const vertex_shader&) = delete;
        vertex_shader(vertex_shader&&) = default;
        ~vertex_shader() = default;

        vertex_shader& operator=(const vertex_shader&) = delete;
        vertex_shader& operator=(vertex_shader&&) = default;

        static vertex_shader compile(const std::filesystem::path& path, std::string function_name,
            std::initializer_list<D3D11_INPUT_ELEMENT_DESC>& input_layout);
        ID3D11VertexShader& get_shader() const noexcept;
        ID3D11InputLayout& get_input_layout() const noexcept;

    private:
        winrt::com_ptr<ID3D11VertexShader> vertex_shader_;
        winrt::com_ptr<ID3D11InputLayout> input_layout_;
    };

	class pixel_shader
	{
	public:
        static void set(pixel_shader& shader);
        static pixel_shader compile(const std::filesystem::path& path, std::string function_name);
	private:
        winrt::com_ptr<ID3D11PixelShader> pixel_shader_;
	};

}
