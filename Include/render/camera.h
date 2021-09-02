// camera.h
// Author: Ryoma Kawamata

#pragma once

#include <DirectXMath.h>

namespace lycoris::render
{
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

		// reset
		void initialize();
		void on_tick();
		// call after OnTick(s) to update view and projection matrix
		void set();

	private:
		DirectX::XMFLOAT4X4 view_matrix_ = {};
		DirectX::XMFLOAT4X4 inverted_view_matrix_ = {};
		DirectX::XMFLOAT4X4 projection_matrix_ = {};

		DirectX::XMFLOAT3 looking_at_ = {};
		DirectX::XMFLOAT3 up_ = {};
		DirectX::XMFLOAT3 position_ = {};
		DirectX::XMFLOAT3 rotation_ = {};
		float distance_ = 10.0f;
	};
}
