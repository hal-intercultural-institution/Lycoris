#pragma once

#include <cstdint>

#include <d3d11.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <DirectXMath.h>
#include <winrt/base.h>

#include "animation.h"
#include "render/texture.h"
#include "render/constantbuffer.h"
#include "render/shader.h"
#include "render/camera.h"
#include "render/text.h"

namespace lycoris::render
{
	constexpr auto animation_max = 16;

	constexpr float color_of(const std::uint8_t color)
	{
		return static_cast<float>(color) / 255.0f;
	}
	
	enum class blend_state
	{
		none,
		alpha,
		add,
		subtract
	};

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT2 tex_coord;
	};

	struct animated_vertex
	{

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT2 tex_coord;
		std::uint32_t anim_index;
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
		screen() noexcept : screen_width_(1280), screen_height_(720), activation_(false), window_handle_(nullptr) {}

		screen(const screen&) = delete;
		screen& operator=(const screen&) = delete;
		
		screen(screen&& other) noexcept = default;
		~screen() = default;
		
		void resize(std::uint32_t new_width, std::uint32_t new_height);

		void set_activation(bool activation) noexcept;
		void set_window_handle(HWND window_handle);
		
		float get_screen_width() const;
		float get_screen_height() const;
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
		// �����_���[�̏���������
		void initialize(HINSTANCE hInstance, HWND hWnd, bool bWindow);
		// �����_���[�̔j������
		void destroy() const;
		// Device���擾
		ID3D11Device& get_device() const;
		// DeviceContext���擾
		ID3D11DeviceContext& get_device_context() const;
		// Depth (Z) buffer ���g�p���邩�ǂ���
		void set_depth_enabled(bool flag) const;
		// ��ʃN���A
		void clear() const;
		// �����_�[�^�[�Q�b�g���t�����g�o�b�t�@�ɓ]��
		void present() const;
		// ���ˉe�ϊ��s��𐶐����AWorld, View, Projection Matrix ���X�V���AGPU�֓]������
		void set_world_view_projection_2d();
		// ���ˉe�ϊ��s��𐶐����AView, Projection Matrix ���X�V���AGPU�ɓ]������
		void set_view_projection_2d();
		// World Matrix ���X�V���AGPU�֓]������
		void set_world_matrix(const DirectX::XMFLOAT4X4& world_matrix);
		// View Matrix ���X�V���AGPU�֓]������
		void set_view_matrix(const DirectX::XMFLOAT4X4& view_matrix);
		// Projection Matrix ���X�V���AGPU�֓]������
		void set_projection_matrix(const DirectX::XMFLOAT4X4& projection_matrix);
		// ���s�������X�V���AGPU�֓]������
		void set_directional_light(const DirectX::XMFLOAT3& light);
		// Material ���X�V���AGPU�֓]������
		void set_material(const material& material);
		// UV�I�t�Z�b�g���X�V����
		void set_uv_offset(const DirectX::XMFLOAT2& offset);
		// �J�����O�ݒ�
		void set_culling_mode(D3D11_CULL_MODE culling_mode);
		// �w�i�F
		void set_background_color(const DirectX::XMFLOAT4& color);
		// �r���[�|�[�g
		void set_viewport(const viewport& viewport);
		// �Ԃ��ǂ��ā[��
		void set_blend_state(blend_state state);
		// �A�j���[�V�����p
		void set_animation_matrix(std::size_t index, const DirectX::XMFLOAT4X4& matrix);
		// �A�j���[�V�����p (�ꊇ)
		void set_animation(const animation::animator& animator);
		// ���_�V�F�[�_�[�ݒ�
		void set_vertex_shader(shader::vertex shader);
		// �e�L�X�g�`�� (���) (DirectWrite)
		void draw_text(const std::wstring& text, const text_format& format, const text_color& color, const text_canvas& canvas) const;
		// �e�L�X�g�`�� (�e�N�X�`��) (DirectWrite)
		void draw_text(const std::wstring& text, const text_format& format, const text_color& color, const text_canvas& canvas, const texture::texture& texture) const;

		screen& get_screen();
		// get the primary camera
		camera& get_camera();
		// get camera
		std::array<camera, 4>& get_cameras();

		[[nodiscard("Ignoring value returned wastes resources.")]]
		// creates text format object used by text renderer
		text_format create_text_format(const std::wstring& font_name, float size) const;
		[[nodiscard("Ignoring value returned wastes resources.")]]
		// creates text color object used by text renderer
		text_color create_text_color(const DirectX::XMFLOAT4& color) const;

	private:
		// pointers
		
		winrt::com_ptr<ID3D11Device> device_;
		winrt::com_ptr<ID3D11DeviceContext> immediate_context_;
		winrt::com_ptr<IDXGISwapChain> swap_chain_;
		winrt::com_ptr<IDXGIDevice1> dxgi_device_;
		winrt::com_ptr<ID3D11RenderTargetView> render_target_view_;
		winrt::com_ptr<ID3D11DepthStencilView> depth_stencil_view_;

		std::array<shader::vertex_shader, static_cast<std::size_t>(shader::vertex::max)> vertex_shaders_;
		shader::pixel_shader pixel_shader_;

		winrt::com_ptr<ID3D11DepthStencilState> depth_stencil_state_enabled_;
		winrt::com_ptr<ID3D11DepthStencilState> depth_stencil_state_disabled_;
		winrt::com_ptr<ID3D11Texture2D> depth_stencil_texture_;
		winrt::com_ptr<ID3D11RasterizerState> rasterizer_state_;
		std::array<winrt::com_ptr<ID3D11BlendState>, 4> blend_states_;
		winrt::com_ptr<ID3D11SamplerState> sampler_state_;

		winrt::com_ptr<ID2D1Device> d2d_device_;
		winrt::com_ptr<ID2D1DeviceContext> d2d_device_context_;
		winrt::com_ptr<ID2D1Bitmap1> d2d_bitmap_screen_;

		winrt::com_ptr<IDWriteFactory> d_write_factory_;

		// values

		constant_buffer<DirectX::XMFLOAT4X4, 0> world_matrix_ = {};
		constant_buffer<DirectX::XMFLOAT4X4, 1> view_matrix_ = {};
		constant_buffer<DirectX::XMFLOAT4X4, 2> projection_matrix_ = {};
		constant_buffer<material, 3> material_ = {};
		constant_buffer<DirectX::XMFLOAT4, 4> directional_light_ = {};
		constant_buffer<DirectX::XMFLOAT4, 5> uv_offset_ = {};
		constant_buffer<std::array<DirectX::XMFLOAT4X4, animation_max>, 6> anim_matrix_ = {};

		D3D11_CULL_MODE culling_mode_ = D3D11_CULL_BACK;

		std::array<float, 4> background_color_ = { color_of(247), color_of(219), color_of(240), 1.0f };
		
		screen screen_;
		std::array<camera, 4> camera_{};
	};
}
