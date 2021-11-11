// camera.h
// Author: Ryoma Kawamata

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

namespace lycoris::render
{
	class viewport
	{
	public:
		viewport() = default;

		// width = width of viewport, 0.0f ~ 1.0f
		// height = height of viewport, 0.0f ~ 1.0f
		// top_left_x = x of base position, 0.0f ~ 1.0f
		// top_left_y = y of base position, 0.0f ~ 1.0f
		viewport(float width, float height, float top_left_x, float top_left_y);

		const D3D11_VIEWPORT& get_raw() const noexcept;

	private:
		D3D11_VIEWPORT viewport_{};

	};

	class camera
	{
	public:
		// position of the camera
		const DirectX::XMFLOAT3& get_position() const noexcept;
		// position where the camera is looking at
		const DirectX::XMFLOAT3& get_looking_at() const noexcept;
		// rotation
		const DirectX::XMFLOAT3& get_rotation() const noexcept;
		// distance between camera and followee
		float get_camera_distance() const noexcept;
		// viewport
		const viewport& get_viewport() const noexcept;

		const DirectX::XMFLOAT4X4& get_inverted_view_matrix() const noexcept;

		// sets position
		void set_position(const DirectX::XMFLOAT3& position) noexcept;
		//// sets rotation (quaternion)
		//void set_rotation(const DirectX::XMFLOAT4& rotation) noexcept;
		// sets rotation (eular)
		void set_rotation(const DirectX::XMFLOAT3& rotation) noexcept;
		// sets position where looking at
		void set_looking_at(const DirectX::XMFLOAT3& position) noexcept;
		// sets camera distance
		void set_distance(float distance) noexcept;
		// viewport
		void set_viewport(const viewport& viewport) noexcept;

		// is the camera used?
		bool is_used() const noexcept;
		//
		void set_use(bool use) noexcept;

		// reset
		void initialize();
		void on_tick();
		// call after OnTick(s) to update view and projection matrix
		void set();

		explicit operator bool() const
		{
			return used_;
		}

	private:
		DirectX::XMFLOAT4X4 view_matrix_ = {};
		DirectX::XMFLOAT4X4 inverted_view_matrix_ = {};
		DirectX::XMFLOAT4X4 projection_matrix_ = {};

		DirectX::XMFLOAT3 looking_at_ = {};
		DirectX::XMFLOAT3 up_ = {};
		DirectX::XMFLOAT3 position_ = {};
		DirectX::XMFLOAT3 rotation_ = {};
		float distance_ = 10.0f;
		bool used_ = false;

		viewport viewport_{};
	};
}
