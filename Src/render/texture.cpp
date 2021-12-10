#include "render/texture.h"

lycoris::render::texture::image::image(const std::uint32_t width, const std::uint32_t height, const std::uint32_t size_per_pixel)
{
	width_ = width;
	height_ = height;
	size_per_pixel_ = size_per_pixel;
	buffer_ = std::vector<std::uint8_t>(width * height * size_per_pixel);
}

std::uint8_t* lycoris::render::texture::image::get_raw_buffer() noexcept
{
	return buffer_.data();
}

const std::uint8_t* lycoris::render::texture::image::get_buffer() const noexcept
{
	return buffer_.data();
}

std::uint32_t lycoris::render::texture::image::get_width() const noexcept
{
	return width_;
}

std::uint32_t lycoris::render::texture::image::get_height() const noexcept
{
	return height_;
}

std::uint32_t lycoris::render::texture::image::get_size_per_pixel() const noexcept
{
	return size_per_pixel_;
}

lycoris::render::texture::texture::texture(winrt::com_ptr<ID3D11ShaderResourceView>&& shader_resource_view,
	winrt::com_ptr<ID3D11Texture2D>&& texture) noexcept
{
	shader_resource_view_ = std::move(shader_resource_view);
	texture_ = std::move(texture);
}

ID3D11Texture2D* lycoris::render::texture::texture::get_texture() const noexcept
{
	return texture_.get();
}

ID3D11ShaderResourceView* lycoris::render::texture::texture::get_shader_resource_view() const noexcept
{
	return shader_resource_view_.get();
}