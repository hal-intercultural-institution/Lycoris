#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <d2d1_1.h>
#include <winrt/base.h>

namespace lycoris::render
{
	class screen
	{
	public:
		screen() noexcept : screen_width_(1280), screen_height_(720), activation_(false), window_handle_(nullptr) {}
		screen(const screen&) = delete;
		screen(screen&& other) = delete;
		~screen() = default;

		screen& operator=(const screen&) = delete;
		screen& operator=(screen&&) = delete;

		// first initialization
		void initialize(std::uint32_t width, std::uint32_t height);
		void initialize();
		void initialize_d2d();
		// resizes window(screen) manually
		void resize(std::uint32_t new_width, std::uint32_t new_height);
		// resizes only back buffer (for window-procedure)
		void resize(std::uint64_t param);
		void clear(const std::array<float, 4>& color) const;

		void set_activation(bool activation) noexcept;
		void set_window_handle(HWND window_handle);

		float get_screen_width() const;
		float get_screen_height() const;
		bool is_active() const noexcept;
		HWND get_window_handle() const;
		ID2D1Bitmap1& get_d2d_screen() const;

	private:
		// actual implementation of resizing
		void resize();
		float screen_width_;
		float screen_height_;
		winrt::com_ptr<ID3D11RenderTargetView> render_target_view_;
		winrt::com_ptr<ID3D11DepthStencilView> depth_stencil_view_;
		winrt::com_ptr<ID3D11Texture2D> depth_stencil_texture_;
		winrt::com_ptr<ID2D1Bitmap1> d2d_bitmap_screen_;
		bool activation_;
		HWND window_handle_;
	};
}
