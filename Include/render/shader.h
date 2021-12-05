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

        ID3D11VertexShader& get_shader() const noexcept;
        ID3D11InputLayout& get_input_layout() const noexcept;

    private:
        winrt::com_ptr<ID3D11VertexShader> vertex_shader_;
        winrt::com_ptr<ID3D11InputLayout> input_layout_;
    };

    constexpr auto pixel_shader_count = 1;
    enum class pixel
    {
	    normal,
    };

	class pixel_shader
	{
	public:
        pixel_shader() = default;
        pixel_shader(winrt::com_ptr<ID3D11PixelShader>&& pixel_shader);
        pixel_shader(const pixel_shader&) = delete;
        pixel_shader(pixel_shader&&) = default;
        ~pixel_shader() = default;

        pixel_shader& operator=(const pixel_shader&) = delete;
        pixel_shader& operator=(pixel_shader&&) = default;

        ID3D11PixelShader& get_shader() const;

	private:
        winrt::com_ptr<ID3D11PixelShader> pixel_shader_;
	};

}
