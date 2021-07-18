//=============================================================================
//
// レンダリング処理 [renderer.h]
// Author : 
//
//=============================================================================
#pragma once

#include <vector>
#include <cstdint>

#include <d3d11.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <DirectXMath.h>
#include <winrt/base.h>

#include "render/camera.h"

namespace lycoris::render
{
	struct constant
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4 light;
		DirectX::XMFLOAT4 camera;
	};

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
		screen() noexcept : screen_width_(1280), screen_height_(720) {}
		
		screen(const screen&) = delete;
		screen& operator=(const screen&) = delete;
		
		screen(screen&& other) noexcept
		{
			render_target_view_ = { std::move(other.render_target_view_) };
			depth_stencil_view_ = { std::move(other.depth_stencil_view_) };
		}
		
		void resize(std::uint32_t new_width, std::uint32_t new_height);
		float get_screen_width() const;
		float get_screen_height() const;
		ID3D11RenderTargetView* get_render_target_view();
	private:
		float screen_width_;
		float screen_height_;
		winrt::com_ptr<ID3D11RenderTargetView> render_target_view_;
		winrt::com_ptr<ID3D11DepthStencilView> depth_stencil_view_;
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
		// 正射影変換行列を生成し、GPUへ転送する
		void set_world_view_projection_2d();
		// World Matrix を更新し、射影変換行列を生成し、GPUへ転送する
		void set_world_matrix(DirectX::XMFLOAT4X4& world_matrix);
		// View Matrix を更新し、射影変換行列を生成し、GPUへ転送する
		void set_view_matrix(DirectX::XMFLOAT4X4& view_matrix);
		// Projection Matrix を更新し、射影変換行列を生成し、GPUへ転送する
		void set_projection_matrix(DirectX::XMFLOAT4X4& projection_matrix);
		// 平行光源を更新し、GPUへ転送する
		void set_directional_light(DirectX::XMFLOAT3& light);
		// Material を更新し、GPUへ転送する
		void set_material(material& material);

		void draw_text(const std::wstring& text);

		ID3D11VertexShader& get_vertex_shader(std::uint64_t index);
		ID3D11PixelShader& get_pixel_shader(int index);

		screen& get_screen();
		camera& get_camera();

	private:
		winrt::com_ptr<ID3D11Device> device_;
		winrt::com_ptr<ID3D11DeviceContext> immediate_context_;
		winrt::com_ptr<IDXGISwapChain> swap_chain_;
		winrt::com_ptr<IDXGIDevice1> dxgi_device_;
		winrt::com_ptr<ID3D11RenderTargetView> render_target_view_;
		winrt::com_ptr<ID3D11DepthStencilView> depth_stencil_view_;

		std::vector<winrt::com_ptr<ID3D11VertexShader>> vertex_shader_;
		std::vector<winrt::com_ptr<ID3D11PixelShader>> pixel_shader_;
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

		DirectX::XMFLOAT4X4 world_matrix_ = {};
		DirectX::XMFLOAT4X4 view_matrix_ = {};
		DirectX::XMFLOAT4X4 projection_matrix_ = {};
		DirectX::XMFLOAT4 directional_light_ = {};

		screen screen_;
		camera camera_;
	};
}
