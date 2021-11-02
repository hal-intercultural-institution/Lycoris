#pragma once

#include <cstdint>

#include <d3d11.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <DirectXMath.h>
#include <winrt/base.h>

#include "render/constantbuffer.h"
#include "render/shader.h"
#include "render/camera.h"

namespace lycoris::render
{

	constexpr float color_of(const std::uint8_t color)
	{
		return static_cast<float>(color) / 255.0f;
	}
	
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT2 tex_coord;
	};

	struct material
	{
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;
		DirectX::XMFLOAT4 emission;
		float shininess;
		DirectX::XMFLOAT3 padding;
	};

	class screen
	{
	public:
		screen() noexcept
			: screen_width_(1280), screen_height_(720), window_handle_(nullptr) {}
		
		screen(const screen&) = delete;
		screen& operator=(const screen&) = delete;
		
		screen(screen&& other) noexcept = default;
		~screen() = default;
		
		void resize(std::uint32_t new_width, std::uint32_t new_height);

		void set_activation(bool activation) noexcept;
		void set_window_handle(HWND window_handle);
		
		float get_screen_width() const;
		float get_screen_height() const;
		ID3D11RenderTargetView* get_render_target_view();
		bool is_active() const noexcept;
		HWND get_window_handle() const;
	
	private:
		float screen_width_;
		float screen_height_;
		winrt::com_ptr<ID3D11RenderTargetView> render_target_view_;
		winrt::com_ptr<ID3D11DepthStencilView> depth_stencil_view_;
		bool activation_;
		HWND window_handle_;
	};

	class renderer
	{
	public:
		// レンダラーの初期化処理
		void initialize(HINSTANCE hInstance, HWND hWnd, bool bWindow);
		// レンダラーの破棄処理
		void destroy() const;
		// Deviceを取得
		ID3D11Device& get_device() const;
		// DeviceContextを取得
		ID3D11DeviceContext& get_device_context() const;
		// Depth (Z) buffer を使用するかどうか
		void set_depth_enabled(bool flag) const;
		// 画面クリア
		void clear() const;
		// レンダーターゲットをフロントバッファに転送
		void present() const;
		// 正射影変換行列を生成し、World, View, Projection Matrix を更新し、GPUへ転送する
		void set_world_view_projection_2d();
		// 正射影変換行列を生成し、View, Projection Matrix を更新し、GPUに転送する
		void set_view_projection_2d();
		// World Matrix を更新し、GPUへ転送する
		void set_world_matrix(const DirectX::XMFLOAT4X4& world_matrix);
		// View Matrix を更新し、GPUへ転送する
		void set_view_matrix(const DirectX::XMFLOAT4X4& view_matrix);
		// Projection Matrix を更新し、GPUへ転送する
		void set_projection_matrix(const DirectX::XMFLOAT4X4& projection_matrix);
		// 平行光源を更新し、GPUへ転送する
		void set_directional_light(const DirectX::XMFLOAT3& light);
		// Material を更新し、GPUへ転送する
		void set_material(const material& material);
		// UVオフセットを更新する
		void set_uv_offset(const DirectX::XMFLOAT2& offset);
		// カリング設定
		void set_culling_mode(D3D11_CULL_MODE culling_mode);
		// 背景色
		void set_background_color(const DirectX::XMFLOAT4& color);
		// ビューポート
		void set_viewport(const viewport& viewport);
		// テキスト描画 (DirectWrite)
		void draw_text(const std::wstring& text);

		//ID3D11VertexShader& get_vertex_shader(std::uint64_t index);
		//ID3D11PixelShader& get_pixel_shader(int index);

		screen& get_screen();
		// get the primary camera
		camera& get_camera();
		// get camera
		std::array<camera, 4>& get_cameras();

	private:
		// pointers
		
		winrt::com_ptr<ID3D11Device> device_;
		winrt::com_ptr<ID3D11DeviceContext> immediate_context_;
		winrt::com_ptr<IDXGISwapChain> swap_chain_;
		winrt::com_ptr<IDXGIDevice1> dxgi_device_;
		winrt::com_ptr<ID3D11RenderTargetView> render_target_view_;
		winrt::com_ptr<ID3D11DepthStencilView> depth_stencil_view_;

		shader::vertex_shader vertex_shader_;
		shader::pixel_shader pixel_shader_;
		winrt::com_ptr<ID3D11InputLayout> input_layout_;
		winrt::com_ptr<ID3D11Buffer> constant_buffer_;
		winrt::com_ptr<ID3D11Buffer> material_buffer_;

		winrt::com_ptr<ID3D11DepthStencilState> depth_stencil_state_enabled_;
		winrt::com_ptr<ID3D11DepthStencilState> depth_stencil_state_disabled_;
		winrt::com_ptr<ID3D11Texture2D> depth_stencil_texture_;
		winrt::com_ptr<ID3D11RasterizerState> rasterizer_state_;
		winrt::com_ptr<ID3D11BlendState> blend_state_;
		winrt::com_ptr<ID3D11SamplerState> sampler_state_;

		winrt::com_ptr<ID2D1Device> d2d_device_;
		winrt::com_ptr<ID2D1DeviceContext> d2d_device_context_;
		winrt::com_ptr<ID2D1Bitmap1> d2d_bitmap_;
		winrt::com_ptr<IDWriteTextFormat> d_write_text_format_;

		// values

		constant_buffer<DirectX::XMFLOAT4X4, 0> world_matrix_ = {};
		constant_buffer<DirectX::XMFLOAT4X4, 1> view_matrix_ = {};
		constant_buffer<DirectX::XMFLOAT4X4, 2> projection_matrix_ = {};
		constant_buffer<material, 3> material_ = {};
		constant_buffer<DirectX::XMFLOAT4, 4> directional_light_ = {};
		constant_buffer<DirectX::XMFLOAT4, 5> uv_offset_ = {};

		D3D11_CULL_MODE culling_mode_ = D3D11_CULL_BACK;

		std::array<float, 4> background_color_ = { color_of(247), color_of(219), color_of(240), 1.0f };
		
		screen screen_;
		std::array<camera, 4> camera_{};
	};
}
