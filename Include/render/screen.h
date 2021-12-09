#pragma once

#include <Windows.h>
#include <d3d11.h>
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
}
