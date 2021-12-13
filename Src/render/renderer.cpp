#include "render/renderer.h"

#include <d3dcompiler.h>

#include <stdexcept>
#include <array>

#include "game.h"
#include "render/shader/ps.h"
#include "render/shader/ps_alpha.h"
#include "render/shader/vs.h"
#include "render/shader/vs_anim.h"

namespace
{
#ifdef _DEBUG
	constexpr std::uint32_t shader_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	constexpr std::uint32_t shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif
	constexpr auto vertex_shader_version = "vs_4_0";
	constexpr auto pixel_shader_version = "ps_4_0";

	constexpr std::array blend_factor = { 0.0f, 0.0f, 0.0f, 0.0f };
	constexpr auto locale = L"ja-JP";

}

ID3D11Device& lycoris::render::renderer::get_device() const
{
	return *device_.get();
}

ID3D11DeviceContext& lycoris::render::renderer::get_device_context() const
{
	return *immediate_context_.get();
}

IDXGISwapChain& lycoris::render::renderer::get_swap_chain() const
{
	return *swap_chain_.get();
}

ID2D1Device& lycoris::render::renderer::get_2d_device() const
{
	return *d2d_device_;
}

ID2D1DeviceContext& lycoris::render::renderer::get_2d_device_context() const
{
	return *d2d_device_context_;
}

lycoris::render::camera& lycoris::render::renderer::get_camera()
{
	return camera_.at(0);
}

std::array<lycoris::render::camera, 4>& lycoris::render::renderer::get_cameras()
{
	return camera_;
}

lycoris::render::text_format lycoris::render::renderer::create_text_format(const std::wstring& font_name, const float size) const
{
	winrt::com_ptr<IDWriteTextFormat> format;
	d_write_factory_->CreateTextFormat(
		font_name.c_str(), nullptr,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		size, locale, format.put());
	return text_format(std::move(format));
}

lycoris::render::text_color lycoris::render::renderer::create_text_color(const DirectX::XMFLOAT4& color) const
{
	winrt::com_ptr<ID2D1SolidColorBrush> brush;
	d2d_device_context_->CreateSolidColorBrush(D2D1::ColorF(color.x, color.y, color.z, color.w), brush.put());
	return text_color(std::move(brush));
}

lycoris::render::shader::vertex_shader lycoris::render::renderer::compile_vertex_shader(
	const std::filesystem::path& path, const std::string& function_name,
	const std::initializer_list<D3D11_INPUT_ELEMENT_DESC>& input_layout) const
{
	winrt::com_ptr<ID3DBlob> vs_binary, error_message;
	const HRESULT hr = D3DCompileFromFile(path.wstring().c_str(), nullptr, nullptr, 
		function_name.c_str(), vertex_shader_version, shader_flags, 0, vs_binary.put(), error_message.put());
	if (FAILED(hr))
	{
		const auto error = static_cast<char*>(error_message->GetBufferPointer());
		throw std::runtime_error("Renderer: failed to compile vertex shader: " + std::string(error));
	}

	return compile_vertex_shader(vs_binary->GetBufferPointer(), vs_binary->GetBufferSize(), input_layout);
}

lycoris::render::shader::vertex_shader lycoris::render::renderer::compile_vertex_shader(const void* byte_code,
	const std::size_t length, const std::initializer_list<D3D11_INPUT_ELEMENT_DESC>& input_layout) const
{
	winrt::com_ptr<ID3D11VertexShader> shader_ptr;
	winrt::com_ptr<ID3D11InputLayout> layout_ptr;

	device_->CreateVertexShader(byte_code, length, nullptr, shader_ptr.put());
	const auto layout = std::vector(input_layout);

	device_->CreateInputLayout(layout.data(),
		static_cast<std::uint32_t>(layout.size()), byte_code, length, layout_ptr.put());

	return shader::vertex_shader(std::move(shader_ptr), std::move(layout_ptr));
}

lycoris::render::shader::pixel_shader lycoris::render::renderer::compile_pixel_shader(const std::filesystem::path& path,
	const std::string& function_name) const
{
	winrt::com_ptr<ID3D11PixelShader> shader_ptr;
	winrt::com_ptr<ID3DBlob> ps_binary, error_message;
	if (FAILED(D3DCompileFromFile(path.wstring().c_str(), nullptr, nullptr,
		function_name.c_str(), pixel_shader_version, shader_flags, 0, ps_binary.put(), error_message.put())))
	{
		const auto error = static_cast<char*>(error_message->GetBufferPointer());
		throw std::runtime_error("PixelShader: failed to compile: " + std::string(error));
	}

	return compile_pixel_shader(ps_binary->GetBufferPointer(), ps_binary->GetBufferSize());
}

lycoris::render::shader::pixel_shader lycoris::render::renderer::compile_pixel_shader(const void* byte_code,
	const std::size_t length) const
{
	winrt::com_ptr<ID3D11PixelShader> shader_ptr;

	device_->CreatePixelShader(byte_code, length, nullptr, shader_ptr.put());

	return shader::pixel_shader(std::move(shader_ptr));
}

lycoris::render::screen& lycoris::render::renderer::get_screen()
{
	return screen_;
}

void lycoris::render::renderer::set_depth_enabled(const bool flag)
{
	const auto state = flag ? depth_stencil_state::depth : depth_stencil_state::none;
	set_depth_stencil_state(state);
}

void lycoris::render::renderer::set_depth_stencil_state(const depth_stencil_state state)
{
	if (depth_stencil_state_ == state) return;

	depth_stencil_state_ = state;
	immediate_context_->OMSetDepthStencilState(depth_stencil_states_[static_cast<std::size_t>(depth_stencil_state_)].get(), 0);
}

void lycoris::render::renderer::set_world_view_projection_2d()
{
	const auto identified_matrix = DirectX::XMMatrixIdentity();
	
	XMStoreFloat4x4(&world_matrix_.get(), identified_matrix);
	world_matrix_.update();

	XMStoreFloat4x4(&view_matrix_.get(), identified_matrix);
	view_matrix_.update();

	const auto height = screen_.get_screen_height();
	const auto width = screen_.get_screen_width();
	
	// Ortho (正) 射影変換行列を生成する
	const auto world_view_projection = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&projection_matrix_.get(), XMMatrixTranspose(world_view_projection));
	projection_matrix_.update();

}

void lycoris::render::renderer::set_view_projection_2d()
{
	const auto identified_matrix = DirectX::XMMatrixIdentity();
	
	XMStoreFloat4x4(&view_matrix_.get(), identified_matrix);
	view_matrix_.update();

	const auto height = screen_.get_screen_height();
	const auto width = screen_.get_screen_width();

	// Ortho (正) 射影変換行列を生成する
	const auto world_view_projection = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&projection_matrix_.get(), XMMatrixTranspose(world_view_projection));
	projection_matrix_.update();
}

void lycoris::render::renderer::set_world_matrix(const DirectX::XMFLOAT4X4& world_matrix)
{
	XMStoreFloat4x4(&world_matrix_.get(), XMMatrixTranspose(XMLoadFloat4x4(&world_matrix)));
	world_matrix_.update();
}

void lycoris::render::renderer::set_view_matrix(const DirectX::XMFLOAT4X4& view_matrix)
{
	XMStoreFloat4x4(&view_matrix_.get(), XMMatrixTranspose(XMLoadFloat4x4(&view_matrix)));
	view_matrix_.update();
}

void lycoris::render::renderer::set_projection_matrix(const DirectX::XMFLOAT4X4& projection_matrix)
{
	XMStoreFloat4x4(&projection_matrix_.get(), XMMatrixTranspose(XMLoadFloat4x4(&projection_matrix)));
	projection_matrix_.update();
}

void lycoris::render::renderer::set_directional_light(const DirectX::XMFLOAT3& light)
{
	directional_light_.update({ light.x, light.y, light.z, 0.0f });
}

void lycoris::render::renderer::set_material(const material& material)
{
	material_.update(material);
}

void lycoris::render::renderer::set_uv_offset(const DirectX::XMFLOAT2& offset)
{
	uv_offset_.update({ offset.x, offset.y, 0.0f, 0.0f });
}

void lycoris::render::renderer::set_culling_mode(const culling_mode culling_mode)
{
	if (culling_mode_ == culling_mode) return;
	culling_mode_ = culling_mode;
	
	immediate_context_->RSSetState(rasterizer_states_[static_cast<std::size_t>(culling_mode_)].get());
}

void lycoris::render::renderer::set_background_color(const DirectX::XMFLOAT4& color)
{
	background_color_ = { color.x, color.y, color.z, color.w };
}

void lycoris::render::renderer::set_viewport(const viewport& viewport)
{
	const std::array viewports = {
		viewport.get()
	};
	immediate_context_->RSSetViewports(static_cast<std::uint32_t>(viewports.size()), viewports.data());
}

void lycoris::render::renderer::set_blend_state(const blend_state state)
{
	if (blend_state_ == state) return;
	blend_state_ = state;

	immediate_context_->OMSetBlendState(blend_states_[static_cast<std::size_t>(blend_state_)].get(),
		blend_factor.data(), 0xffffffff);
}

void lycoris::render::renderer::set_alpha_test_state(const bool is_enabled)
{
	const shader::pixel incoming = is_enabled ? shader::pixel::alpha_test : shader::pixel::normal;
	if (pixel_shader_ == incoming) return;
	set_pixel_shader(incoming);
}

void lycoris::render::renderer::set_animation_matrix(const std::size_t index, const DirectX::XMFLOAT4X4& matrix)
{
	anim_matrix_.get()[index] = matrix;
	anim_matrix_.update();
}

void lycoris::render::renderer::set_animation(const animation::animator& animator)
{
	assert(animator.get().size() <= animation_max);

	for (std::size_t i = 0; i < animator.get().size(); ++i)
	{
		const auto& animation = animator.get().at(i);
		auto matrix = XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&animation.get_scale())));
		matrix = XMMatrixMultiply(matrix, DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&animation.get_rotation())));
		matrix = XMMatrixMultiply(matrix, DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&animation.get_position())));
		matrix = XMMatrixTranspose(matrix);
		XMStoreFloat4x4(&anim_matrix_.get()[i], matrix);
	}

	anim_matrix_.update();
}

void lycoris::render::renderer::set_vertex_shader(const shader::vertex vertex_shader)
{
	if (vertex_shader_ == vertex_shader) return;
	vertex_shader_ = vertex_shader;

	const auto& shader = vertex_shaders_[static_cast<std::size_t>(vertex_shader_)];
	immediate_context_->VSSetShader(&shader.get_shader(), nullptr, 0);
	immediate_context_->IASetInputLayout(&shader.get_input_layout());
}

void lycoris::render::renderer::set_pixel_shader(const shader::pixel pixel_shader)
{
	if (pixel_shader_ == pixel_shader) return;
	pixel_shader_ = pixel_shader;

	immediate_context_->PSSetShader(&pixel_shaders_[static_cast<std::size_t>(pixel_shader_)].get_shader(), nullptr, 0);
}

void lycoris::render::renderer::draw_text(const std::wstring& text, const text_format& format, const text_color& color,
                                          const text_canvas& canvas) const
{
	d2d_device_context_->SetTarget(&screen_.get_d2d_screen());
	d2d_device_context_->BeginDraw();
	d2d_device_context_->DrawText(text.c_str(), static_cast<std::uint32_t>(text.size()), &format.get(), &canvas.get(), &color.get());
	d2d_device_context_->EndDraw();
}

void lycoris::render::renderer::draw_text(const std::wstring& text, const text_format& format, const text_color& color,
	const text_canvas& canvas, const texture::texture& texture) const
{
	winrt::com_ptr<IDXGISurface> surface;
	winrt::com_ptr<ID2D1Bitmap1> bitmap;

	HRESULT result = texture.get_texture()->QueryInterface(IID_PPV_ARGS(surface.put()));
	if (FAILED(result))
		throw std::runtime_error("TextRenderer: failed to create DXGI Surface from Direct3D Texture");

	const auto dpi = static_cast<float>(GetDpiForWindow(screen_.get_window_handle()));
	const D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpi, dpi
	);

	result = d2d_device_context_->CreateBitmapFromDxgiSurface(surface.get(), &bitmap_properties, bitmap.put());
	if (FAILED(result))
		throw std::runtime_error("TextRenderer: failed to create Direct2D Bitmap from DXGI Surface");
	d2d_device_context_->SetTarget(bitmap.get());

	d2d_device_context_->BeginDraw();
	d2d_device_context_->DrawText(text.c_str(), static_cast<std::uint32_t>(text.size()), &format.get(), &canvas.get(), &color.get());
	d2d_device_context_->EndDraw();

}

void lycoris::render::renderer::initialize(HINSTANCE hInstance, HWND hWnd, bool bWindow)
{
	const auto& settings = game::get_game().get_launch_settings();
	HRESULT hr = S_OK;

	screen_.set_window_handle(hWnd);

	// Device, SwapChain, ImmediateContext 生成
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = settings.screen_width;
	swap_chain_desc.BufferDesc.Height = settings.screen_height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = hWnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = true;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	std::array feature_level {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	
	std::uint32_t flags = 0;
	flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	
#if _DEBUG
	// debug layer
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	hr = D3D11CreateDeviceAndSwapChain(nullptr, // display adapter
	                                   D3D_DRIVER_TYPE_HARDWARE, // driver
	                                   nullptr, // software driver type
	                                   flags, // flags
	                                   feature_level.data(), // feature levels
	                                   static_cast<std::uint32_t>(feature_level.size()), // count
	                                   D3D11_SDK_VERSION, // DirectX SDK Version
	                                   &swap_chain_desc, // DXGI_SWAP_CHAIN_DESC
	                                   swap_chain_.put(), // SwapChain output
	                                   device_.put(), // Device output
	                                   nullptr, // feature level output
	                                   immediate_context_.put()); // DeviceContext output

	if (FAILED(hr))
		throw std::runtime_error("Renderer: failed to create d3d device.");

	// バックバッファ、深度とステンシルの初期化
	screen_.initialize(settings.screen_width, settings.screen_height);

	// カメラとviewport
	{
		camera_[0].set_use(true);
		camera_[0].set_viewport({ 1.0f, 1.0f, 0.0f, 0.0f });
		set_viewport(camera_[0].get_viewport());
	}

	// Rasterizer State (ピクセルシェーダーの前に実行される、ラスタライズ)
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAME
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.MultisampleEnable = false;
		device_->CreateRasterizerState(&rasterizer_desc, rasterizer_states_[static_cast<std::size_t>(culling_mode::none)].put());

		rasterizer_desc.CullMode = D3D11_CULL_FRONT;
		device_->CreateRasterizerState(&rasterizer_desc, rasterizer_states_[static_cast<std::size_t>(culling_mode::front)].put());

		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		device_->CreateRasterizerState(&rasterizer_desc, rasterizer_states_[static_cast<std::size_t>(culling_mode::back)].put());

		immediate_context_->RSSetState(rasterizer_states_[static_cast<std::size_t>(culling_mode::back)].get());
	}

	// Blend State (ピクセルシェーダーの後 既にあるピクセルの値とのブレンドの仕方を決定する)
	{
		D3D11_BLEND_DESC blend_desc = {};
		blend_desc.AlphaToCoverageEnable = false;
		blend_desc.IndependentBlendEnable = false;
		blend_desc.RenderTarget[0].BlendEnable = true;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		// blend state: none
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		device_->CreateBlendState(&blend_desc, blend_states_[static_cast<std::size_t>(blend_state::none)].put());

		// blend state: alpha
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		device_->CreateBlendState(&blend_desc, blend_states_[static_cast<std::size_t>(blend_state::alpha)].put());

		// blend state: add
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		device_->CreateBlendState(&blend_desc, blend_states_[static_cast<std::size_t>(blend_state::add)].put());

		// blend state: subtract
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		device_->CreateBlendState(&blend_desc, blend_states_[static_cast<std::size_t>(blend_state::subtract)].put());

		immediate_context_->OMSetBlendState(blend_states_[static_cast<std::size_t>(blend_state::alpha)].get(),
			blend_factor.data(), 0xffffffff);
	}

	// Depth Stencil State
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
		depth_stencil_desc.DepthEnable = true;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = false;

		device_->CreateDepthStencilState(&depth_stencil_desc,
			depth_stencil_states_[static_cast<std::size_t>(depth_stencil_state::depth)].put()); // 深度有効ステート

		depth_stencil_desc.DepthEnable = false;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		device_->CreateDepthStencilState(&depth_stencil_desc,
			depth_stencil_states_[static_cast<std::size_t>(depth_stencil_state::none)].put()); // 深度無効ステート

		immediate_context_->OMSetDepthStencilState(depth_stencil_states_[static_cast<std::size_t>(depth_stencil_state::depth)].get(), 0);
	}

	// SamplerState
	{
		D3D11_SAMPLER_DESC sampler_desc = {};
		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

		device_->CreateSamplerState(&sampler_desc, sampler_state_.put());

		std::array samplers = {
			sampler_state_.get()
		};

		immediate_context_->PSSetSamplers(0, static_cast<std::uint32_t>(samplers.size()), samplers.data());
	}

	// Vertex Shader, Input Layout (normal)
	{
		std::initializer_list<D3D11_INPUT_ELEMENT_DESC> layout = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		auto& shader = vertex_shaders_[static_cast<std::size_t>(shader::vertex::normal)];
		shader = compile_vertex_shader(g_vs_main, sizeof g_vs_main, layout);
		immediate_context_->VSSetShader(&shader.get_shader(), nullptr, 0);
		immediate_context_->IASetInputLayout(&shader.get_input_layout());
	}

	// Vertex Shader, Input Layout (animated)
	{
		std::initializer_list<D3D11_INPUT_ELEMENT_DESC> layout = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BLENDINDICES", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		vertex_shaders_[static_cast<std::size_t>(shader::vertex::animated)] = compile_vertex_shader(g_vs_anim, sizeof g_vs_anim, layout);
	}

	// PixelShader (normal)
	{
		auto& shader = pixel_shaders_[static_cast<std::size_t>(shader::pixel::normal)];
		shader = compile_pixel_shader(g_ps_main, sizeof g_ps_main);
		immediate_context_->PSSetShader(&shader.get_shader(), nullptr, 0);
	}

	{
		auto& shader = pixel_shaders_[static_cast<std::size_t>(shader::pixel::alpha_test)];
		shader = compile_pixel_shader(g_ps_alpha, sizeof g_ps_alpha);
	}
	
	// Constant Buffers
	{
		world_matrix_ = constant_buffer<DirectX::XMFLOAT4X4, 0>::create();
		view_matrix_ = constant_buffer<DirectX::XMFLOAT4X4, 1>::create();
		projection_matrix_ = constant_buffer<DirectX::XMFLOAT4X4, 2>::create();
		material_ = constant_buffer<material, 3>::create();
		directional_light_ = constant_buffer<DirectX::XMFLOAT4, 4>::create();
		uv_offset_ = constant_buffer<DirectX::XMFLOAT4, 5>::create();
		anim_matrix_ = constant_buffer<std::array<DirectX::XMFLOAT4X4, animation_max>, 6>::create();
	}
	
	{
		hr = device_->QueryInterface(IID_PPV_ARGS(dxgi_device_.put()));
		dxgi_device_->SetMaximumFrameLatency(1u);
		
		winrt::com_ptr<ID2D1Factory1> d2d_factory;
		D2D1_FACTORY_OPTIONS d2d_factory_options{};
		d2d_factory_options.debugLevel = D2D1_DEBUG_LEVEL_ERROR;
		
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &d2d_factory_options, d2d_factory.put_void());
		if (FAILED(hr))
		{
			throw std::runtime_error("Renderer: failed to initialize d2d factory");
		}


		hr = d2d_factory->CreateDevice(dxgi_device_.get(), d2d_device_.put());
		if (FAILED(hr))
		{
			throw std::runtime_error("Renderer: failed to initialize d2d device");
		}

		hr = d2d_device_->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2d_device_context_.put());
		if (FAILED(hr))
		{
			throw std::runtime_error("Renderer: failed to initialize d2d device context");
		}
	}

	screen_.initialize_d2d();
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(d_write_factory_.get()), reinterpret_cast<IUnknown**>(d_write_factory_.put()));

}

void lycoris::render::renderer::destroy() const
{
	auto& immediate_context = *immediate_context_.get();
	immediate_context.ClearState();
	immediate_context.Flush();
}

void lycoris::render::renderer::clear() const
{
	screen_.clear(background_color_);
}

void lycoris::render::renderer::present() const
{
	swap_chain_->Present(0, 0);
}
