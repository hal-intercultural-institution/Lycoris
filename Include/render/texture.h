#pragma once

#include <vector>
#include <filesystem>

#include <winrt/base.h>
#include <wincodec.h>
#include <d3d11.h>

#include "utility/cast.h"

namespace lycoris::render::texture
{
	using utility::scast::uint64_of;
	class image
	{
	public:
		image(std::uint32_t width, std::uint32_t height, std::uint32_t size_per_pixel);

		std::uint8_t* get_raw_buffer() noexcept;
		const std::uint8_t* get_buffer() const noexcept;
		std::uint32_t get_width() const noexcept;
		std::uint32_t get_height() const noexcept;
		std::uint32_t get_size_per_pixel() const noexcept;

	private:
		std::vector<std::uint8_t> buffer_;
		std::uint32_t width_;
		std::uint32_t height_;
		std::uint32_t size_per_pixel_;
	};

	class texture
	{
	public:
		// construct with nullptr
		texture() = default;

		// construct with pointers
		texture(ID3D11ShaderResourceView* shader_resource_view, ID3D11Texture2D* texture) noexcept
		{
			shader_resource_view_.attach(shader_resource_view);
			texture_.attach(texture);
		}

		texture(const texture&) = default;
		texture(texture&& other) = default;

		texture& operator=(const texture&) = default;
		texture& operator=(texture&& other) = default;

		ID3D11Texture2D* get_texture() const noexcept
		{
			//if (texture_)
			//	throw std::runtime_error("Texture: texture_ is nullptr.");
			return texture_.get();
		}

		ID3D11Texture2D** put_texture() noexcept
		{
			return texture_.put();
		}

		ID3D11ShaderResourceView* get_shader_resource_view() const noexcept
		{
			return shader_resource_view_.get();
		}

		ID3D11ShaderResourceView** put_shader_resource_view() noexcept
		{
			return shader_resource_view_.put();
		}

		~texture() = default;

	private:
		winrt::com_ptr<ID3D11Texture2D> texture_;
		winrt::com_ptr<ID3D11ShaderResourceView> shader_resource_view_;
	};

	class texture_loader
	{
	public:
		void initialize();
		image load_image_from_file(const std::filesystem::path& path);
		texture create_texture_from_file(const std::filesystem::path& path);
		void destroy();
	private:
		//winrt::com_ptr<IWICImagingFactory> imaging_factory_;
		IWICImagingFactory* imaging_factory_ = nullptr;
	};
}
