//=============================================================================
//
// レンダリング処理 [renderer.cpp]
// Author : 
//
//=============================================================================
#include "render/renderer.h"

#include <d3dcompiler.h>

#include <stdexcept>
#include <array>

#include "game.h"

ID3D11Device& lycoris::render::renderer::get_device() const
{
	return *device_.get();
}

ID3D11DeviceContext& lycoris::render::renderer::get_device_context() const
{
	return *immediate_context_.get();
}

lycoris::render::camera& lycoris::render::renderer::get_camera()
{
	return camera_;
}

lycoris::render::screen& lycoris::render::renderer::get_screen()
{
	return screen_;
}

void lycoris::render::renderer::set_depth_enabled(bool flag) const
{
	if (flag)
		immediate_context_->OMSetDepthStencilState(depth_stencil_state_enabled_.get(), 0);
	else
		immediate_context_->OMSetDepthStencilState(depth_stencil_state_disabled_.get(), 0);
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

void lycoris::render::renderer::set_world_matrix(DirectX::XMFLOAT4X4& world_matrix)
{
	XMStoreFloat4x4(&world_matrix_.get(), XMMatrixTranspose(XMLoadFloat4x4(&world_matrix)));
	world_matrix_.update();
}

void lycoris::render::renderer::set_view_matrix(DirectX::XMFLOAT4X4& view_matrix)
{
	XMStoreFloat4x4(&view_matrix_.get(), XMMatrixTranspose(XMLoadFloat4x4(&view_matrix)));
	view_matrix_.update();
}

void lycoris::render::renderer::set_projection_matrix(DirectX::XMFLOAT4X4& projection_matrix)
{
	XMStoreFloat4x4(&projection_matrix_.get(), XMMatrixTranspose(XMLoadFloat4x4(&projection_matrix)));
	projection_matrix_.update();
}

void lycoris::render::renderer::set_directional_light(DirectX::XMFLOAT3& light)
{
	directional_light_.update({ light.x, light.y, light.z, 0.0f });
}

void lycoris::render::renderer::set_material(material& material)
{
	material_.update(material);
}

void lycoris::render::renderer::set_uv_offset(DirectX::XMFLOAT2& offset)
{
	uv_offset_.update({ offset.x, offset.y, 0.0f, 0.0f });
}

void lycoris::render::renderer::set_culling_mode(D3D11_CULL_MODE culling_mode)
{
	if (culling_mode == culling_mode_) return;
	culling_mode_ = culling_mode;
	
	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAME
	rasterizer_desc.CullMode = culling_mode;
	rasterizer_desc.DepthClipEnable = true;
	rasterizer_desc.MultisampleEnable = false;
	winrt::com_ptr<ID3D11RasterizerState> rasterizer_state;
	device_->CreateRasterizerState(&rasterizer_desc, rasterizer_state.put());
	rasterizer_state_ = std::move(rasterizer_state);
	
	immediate_context_->RSSetState(rasterizer_state_.get());
}

void lycoris::render::renderer::set_background_color(const DirectX::XMFLOAT4& color)
{
	background_color_ = { color.x, color.y, color.z, color.w };
}

void lycoris::render::renderer::draw_text(const std::wstring& text)
{
	winrt::com_ptr<ID2D1SolidColorBrush> brush;
	d2d_device_context_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brush.put());
	d2d_device_context_->BeginDraw();
	const auto rect = D2D1::RectF(0, 0, 600, 400);
	d2d_device_context_->DrawText(text.c_str(), static_cast<std::uint32_t>(text.size()), d_write_text_format_.get(), &rect, brush.get());
	d2d_device_context_->EndDraw();
}

void lycoris::render::renderer::initialize(HINSTANCE hInstance, HWND hWnd, bool bWindow)
{
	HRESULT hr = S_OK;

	screen_.set_window_handle(hWnd);

	// Device, SwapChain, ImmediateContext 生成
	DXGI_SWAP_CHAIN_DESC   swap_chain_desc = {};
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = static_cast<std::uint32_t>(screen_.get_screen_width());
	swap_chain_desc.BufferDesc.Height = static_cast<std::uint32_t>(screen_.get_screen_height());
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

	{
		// RenderTargetView
		winrt::com_ptr<ID3D11Texture2D> backBuffer = nullptr;
		swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), backBuffer.put_void());
		hr = device_->CreateRenderTargetView(backBuffer.get(), nullptr, render_target_view_.put());
		if (FAILED(hr))
			throw std::runtime_error("Renderer: failed to create RenderTargetView.");

		// Stencil, Depth Texture
		D3D11_TEXTURE2D_DESC texture_desc = {};
		texture_desc.Width = swap_chain_desc.BufferDesc.Width;
		texture_desc.Height = swap_chain_desc.BufferDesc.Height;
		texture_desc.MipLevels = 1;
		texture_desc.ArraySize = 1;
		texture_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // depth に 24bit, Stencil に 8bit
		texture_desc.SampleDesc = swap_chain_desc.SampleDesc;
		texture_desc.Usage = D3D11_USAGE_DEFAULT;
		texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture_desc.CPUAccessFlags = 0;
		texture_desc.MiscFlags = 0;
		hr = device_->CreateTexture2D(&texture_desc, nullptr, depth_stencil_texture_.put());
		if (FAILED(hr))
			throw std::runtime_error("Renderer: failed to create DepthStencilTexture.");

		// DepthStencilView
		D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
		dsv_desc.Format = texture_desc.Format;
		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Flags = 0;
		hr = device_->CreateDepthStencilView(depth_stencil_texture_.get(), &dsv_desc, depth_stencil_view_.put());
		if (FAILED(hr))
			throw std::runtime_error("Renderer: failed to create DepthStencilView.");

		std::array render_targets = {
			(render_target_view_.get())
		};

		immediate_context_->OMSetRenderTargets(1, render_targets.data(), depth_stencil_view_.get());
	}
	
	// Viewport (= 0.0〜1.0 でスクリーン上の座標を表す)
	{
		D3D11_VIEWPORT viewport = {};
		viewport.Width = screen_.get_screen_width();
		viewport.Height = screen_.get_screen_height();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		immediate_context_->RSSetViewports(1, &viewport);
	}

	// Rasterizer State (ピクセルシェーダーの前に実行される、ラスタライズ)
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAME
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.MultisampleEnable = false;
		device_->CreateRasterizerState(&rasterizer_desc, rasterizer_state_.put());
		
		immediate_context_->RSSetState(rasterizer_state_.get());
	}

	// Blend State (ピクセルシェーダーの後 既にあるピクセルの値とのブレンドの仕方を決定する)
	{
		D3D11_BLEND_DESC blend_desc = {};
		blend_desc.AlphaToCoverageEnable = false;
		blend_desc.IndependentBlendEnable = false;
		blend_desc.RenderTarget[0].BlendEnable = true;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		std::array blendFactor = { 0.0f, 0.0f, 0.0f, 0.0f };
		device_->CreateBlendState(&blend_desc, blend_state_.put());
		immediate_context_->OMSetBlendState(blend_state_.get(), blendFactor.data(), 0xffffffff);
	}

	// Depth Stencil State
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
		depth_stencil_desc.DepthEnable = true;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = false;

		device_->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state_enabled_.put()); // 深度有効ステート

		depth_stencil_desc.DepthEnable = FALSE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		device_->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state_disabled_.put()); // 深度無効ステート

		immediate_context_->OMSetDepthStencilState(depth_stencil_state_enabled_.get(), 0);
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

		const auto& samplerState = sampler_state_.put();
		device_->CreateSamplerState(&sampler_desc, samplerState);

		immediate_context_->PSSetSamplers(0, 1, samplerState);
	}

	// Vertex Shader, Input Layout
	{
		std::initializer_list<D3D11_INPUT_ELEMENT_DESC> layout = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		vertex_shader_ = shader::vertex_shader::compile("data/shader.hlsl", "vs_main", layout);
		shader::vertex_shader::set(vertex_shader_);
	}

	// PixelShader
	{
		pixel_shader_ = shader::pixel_shader::compile("data/shader.hlsl", "ps_main");
		shader::pixel_shader::set(pixel_shader_);
	}
	
	// Constant Buffers
	{
		world_matrix_ = constant_buffer<DirectX::XMFLOAT4X4, 0>::create();
		view_matrix_ = constant_buffer<DirectX::XMFLOAT4X4, 1>::create();
		projection_matrix_ = constant_buffer<DirectX::XMFLOAT4X4, 2>::create();
		material_ = constant_buffer<material, 3>::create();
		directional_light_ = constant_buffer<DirectX::XMFLOAT4, 4>::create();
		uv_offset_ = constant_buffer<DirectX::XMFLOAT4, 5>::create();
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

	{
		winrt::com_ptr<IDXGISurface> dxgi_surface;
		hr = swap_chain_->GetBuffer(0, IID_PPV_ARGS(dxgi_surface.put()));

		float dpi = static_cast<float>(GetDpiForWindow(hWnd));
		D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpi, dpi
		);

		hr = d2d_device_context_->CreateBitmapFromDxgiSurface(dxgi_surface.get(), &bitmap_properties, d2d_bitmap_.put());
		d2d_device_context_->SetTarget(d2d_bitmap_.get());
	}

	{
		winrt::com_ptr<IDWriteFactory> d_write_factory;
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(d_write_factory.get()), reinterpret_cast<IUnknown**>(d_write_factory.put()));

		d_write_factory->CreateTextFormat(
			L"Consolas", nullptr,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
			16.0f, L"ja-JP", d_write_text_format_.put());
	}

	d_write_text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
}

void lycoris::render::renderer::destroy() const
{
	auto& immediate_context = *immediate_context_.get();
	immediate_context.ClearState();
	immediate_context.Flush();
}

void lycoris::render::renderer::clear() const
{
	// clear buffer
	immediate_context_->ClearRenderTargetView(render_target_view_.get(), background_color_.data());
	// clear depth & stencil
	immediate_context_->ClearDepthStencilView(depth_stencil_view_.get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void lycoris::render::renderer::present() const
{
	swap_chain_->Present(0, 0);
}
