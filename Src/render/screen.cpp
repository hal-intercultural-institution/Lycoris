#include "render/screen.h"

#include "game.h"

void lycoris::render::screen::initialize()
{
	const auto& renderer = game::get_game().get_renderer();
	auto& device = renderer.get_device();
	auto& device_context = renderer.get_device_context();
	auto& swap_chain = renderer.get_swap_chain();

	// RenderTargetView
	winrt::com_ptr<ID3D11Texture2D> back_buffer;
	swap_chain.GetBuffer(0, __uuidof(ID3D11Texture2D), back_buffer.put_void());
	HRESULT hr = device.CreateRenderTargetView(back_buffer.get(), nullptr, render_target_view_.put());
	if (FAILED(hr))
		throw std::runtime_error("Renderer: failed to create RenderTargetView.");

	// Stencil, Depth Texture
	D3D11_TEXTURE2D_DESC texture_desc = {};
	texture_desc.Width = static_cast<std::uint32_t>(screen_width_);
	texture_desc.Height = static_cast<std::uint32_t>(screen_height_);
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // depth ‚É 24bit, Stencil ‚É 8bit
	texture_desc.SampleDesc.Count = 1;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = device.CreateTexture2D(&texture_desc, nullptr, depth_stencil_texture_.put());
	if (FAILED(hr))
		throw std::runtime_error("Renderer: failed to create DepthStencilTexture.");

	// DepthStencilView
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
	dsv_desc.Format = texture_desc.Format;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Flags = 0;
	hr = device.CreateDepthStencilView(depth_stencil_texture_.get(), &dsv_desc, depth_stencil_view_.put());
	if (FAILED(hr))
		throw std::runtime_error("Renderer: failed to create DepthStencilView.");

	const std::array render_targets = {
		(render_target_view_.get())
	};

	device_context.OMSetRenderTargets(static_cast<uint32_t>(render_targets.size()), render_targets.data(), depth_stencil_view_.get());
}

void lycoris::render::screen::initialize_d2d()
{
	const auto& renderer = game::get_game().get_renderer();
	auto& swap_chain = renderer.get_swap_chain();
	auto& d2d_device_context = renderer.get_2d_device_context();
	winrt::com_ptr<IDXGISurface> dxgi_surface;
	HRESULT hr = swap_chain.GetBuffer(0, IID_PPV_ARGS(dxgi_surface.put()));

	const auto dpi = static_cast<float>(GetDpiForWindow(window_handle_));
	const D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpi, dpi
	);

	hr = d2d_device_context.CreateBitmapFromDxgiSurface(dxgi_surface.get(), &bitmap_properties, d2d_bitmap_screen_.put());

}

void lycoris::render::screen::resize(const std::uint32_t new_width, const std::uint32_t new_height)
{
	const auto& game = game::get_game();

	const auto wid = static_cast<float>(new_width), hei = static_cast<float>(new_height);
	if (screen_width_ == wid && screen_width_ == hei) return;

	screen_width_ = wid;
	screen_height_ = hei;

	resize();
	SetWindowPos(window_handle_, nullptr, CW_USEDEFAULT, CW_USEDEFAULT,
		new_width + game.get_window_border_x(), new_height + game.get_window_border_y(), SWP_NOMOVE);
}

void lycoris::render::screen::resize(std::uint64_t param)
{
	const auto wid = static_cast<float>(param & 0xFFFF), hei = static_cast<float>(param >> 16 & 0xFFFF);
	if (screen_width_ == wid && screen_width_ == hei) return;

	screen_width_ = wid;
	screen_height_ = hei;

	resize();
}

void lycoris::render::screen::clear(const std::array<float, 4>& color) const
{
	auto& device_context = game::get_game().get_renderer().get_device_context();
	// clear buffer
	device_context.ClearRenderTargetView(render_target_view_.get(), color.data());
	// clear depth & stencil
	device_context.ClearDepthStencilView(depth_stencil_view_.get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void lycoris::render::screen::set_activation(const bool activation) noexcept
{
	activation_ = activation;
}

void lycoris::render::screen::set_window_handle(HWND window_handle)
{
	window_handle_ = window_handle;
}

float lycoris::render::screen::get_screen_width() const
{
	return screen_width_;
}

float lycoris::render::screen::get_screen_height() const
{
	return screen_height_;
}

bool lycoris::render::screen::is_active() const noexcept
{
	return activation_;
}

HWND lycoris::render::screen::get_window_handle() const
{
	return window_handle_;
}

ID2D1Bitmap1& lycoris::render::screen::get_d2d_screen() const
{
	return *d2d_bitmap_screen_;
}

void lycoris::render::screen::resize()
{
	const auto& renderer = game::get_game().get_renderer();

	const std::array<ID3D11RenderTargetView*, 1> targets = {
		nullptr
	};
	renderer.get_device_context().OMSetRenderTargets(
		static_cast<std::uint32_t>(targets.size()), targets.data(), nullptr);

	renderer.get_2d_device_context().SetTarget(nullptr);
	render_target_view_ = nullptr;
	depth_stencil_texture_ = nullptr;
	depth_stencil_view_ = nullptr;
	d2d_bitmap_screen_ = nullptr;

	const HRESULT hr = renderer.get_swap_chain().ResizeBuffers(1, static_cast<std::uint32_t>(screen_width_),
		static_cast<std::uint32_t>(screen_height_), DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	if (FAILED(hr)) throw std::runtime_error("Renderer: failed to resize buffer");

	initialize();
	initialize_d2d();
}
