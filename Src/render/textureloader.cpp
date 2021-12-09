#include "render/texture.h"

#include <stdexcept>

#include "game.h"
#include "gamedef.h"

lycoris::render::texture::image lycoris::render::texture::texture_loader::load_image_from_file(const std::filesystem::path& path)
{
	if (!imaging_factory_)
		throw std::runtime_error("TextureLoader: failed to load texture: factory is not initialized");

	if (!exists(path))
	{
		throw std::runtime_error("TextureLoader: failed to load image: file not found: " + path.string());
	}

	winrt::com_ptr<IWICBitmapDecoder> decoder = nullptr;
	if (FAILED(imaging_factory_->CreateDecoderFromFilename(
		path.wstring().c_str(), nullptr, GENERIC_READ, WICDecodeOptions::WICDecodeMetadataCacheOnDemand, decoder.put())))
		throw std::runtime_error("TextureLoader: failed to create decoder: " + path.string());

	winrt::com_ptr<IWICBitmapFrameDecode> frame = nullptr;
	if (FAILED(decoder->GetFrame(0, frame.put())))
		throw std::runtime_error("TextureLoader: failed to get frame: " + path.string());

	std::uint32_t width, height;
	if (FAILED(frame->GetSize(&width, &height)))
		throw std::runtime_error("TextureLoader: failed to get image size: " + path.string());

	WICPixelFormatGUID pixel_format;
	if (FAILED(frame->GetPixelFormat(&pixel_format)))
		throw std::runtime_error("TextureLoader: failed to get pixelformat: " + path.string());

	constexpr std::uint32_t size_per_pixel = 4U; // R8G8B8A8 = 32bit = 4byte
	if (pixel_format == GUID_WICPixelFormat32bppRGBA) // 32bppRGBA = R8G8B8A8_UNORM
	{
		auto img = image(width, height, size_per_pixel);
		if (FAILED(frame->CopyPixels(nullptr, width * size_per_pixel, width * height * size_per_pixel, img.get_raw_buffer())))
			throw std::runtime_error("TextureLoader: failed to copy data: " + path.string());
		return img;
	}
	winrt::com_ptr<IWICFormatConverter> converter = nullptr;
	if (FAILED(imaging_factory_->CreateFormatConverter(converter.put())))
		throw std::runtime_error("TextureLoader: failed to create converter: " + path.string());

	if (FAILED(converter->Initialize(frame.get(), GUID_WICPixelFormat32bppRGBA,
		WICBitmapDitherType::WICBitmapDitherTypeErrorDiffusion, nullptr, 0.0, WICBitmapPaletteType::WICBitmapPaletteTypeCustom)))
		throw std::runtime_error("TextureLoader: failed to convert image: " + path.string());

	auto img = image(width, height, size_per_pixel);
	if (FAILED(converter->CopyPixels(nullptr, width * size_per_pixel, width * height * size_per_pixel, img.get_raw_buffer())))
		throw std::runtime_error("TextureLoader: failed to copy data: " + path.string());

	return img;
}

lycoris::render::texture::texture_loader::operator bool() const noexcept
{
	return imaging_factory_ != nullptr;
}

void lycoris::render::texture::texture_loader::initialize()
{
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
	                                  IID_PPV_ARGS(&imaging_factory_))))
		throw std::runtime_error("TextureLoader: failed to initialize WICImagingFactory");
}

void lycoris::render::texture::texture_loader::destroy()
{
	imaging_factory_->Release();
}

lycoris::render::texture::texture lycoris::render::texture::texture_loader::create_texture_from_file(const std::filesystem::path& path)
{
	const auto image = load_image_from_file(path);
	auto& renderer = game::get_game().get_renderer();
	auto& device = renderer.get_device();

	winrt::com_ptr<ID3D11Texture2D> texture_ptr;
	winrt::com_ptr<ID3D11ShaderResourceView> shader_resource_view_ptr;

	// Texture2DÇçÏÇÈ
	D3D11_TEXTURE2D_DESC tex_desc;
	tex_desc.Width = image.get_width();
	tex_desc.Height = image.get_height();
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initial_data{};
	initial_data.pSysMem = image.get_buffer();
	initial_data.SysMemPitch = tex_desc.Width * image.get_size_per_pixel();

	HRESULT hr = device.CreateTexture2D(&tex_desc, &initial_data, texture_ptr.put());
	if (FAILED(hr))
		throw std::runtime_error("TextureLoader: failed to create Texture2D");

	// ShaderResourceViewÇçÏÇÈ
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format = tex_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Texture2D.MostDetailedMip = 0;

	hr = device.CreateShaderResourceView(texture_ptr.get(), &srv_desc, shader_resource_view_ptr.put());
	if (FAILED(hr))
		throw std::runtime_error("TextureLoader: failed to create ShaderResourceView");

	return texture(std::move(shader_resource_view_ptr), std::move(texture_ptr));
}
