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
		DirectX::XMFLOAT3& get_looking_at() const noexcept;
		// distance between camera and followee
		float& get_camera_distance() const noexcept;

		// reset
		void initialize();
		// call this to handle mouse movement or joystick tilt to rotate camera
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
		DirectX::XMFLOAT4 rotation_ = {};
		float distance_ = 10.0f;
		// isFollowingEntity_
		// followingEntity_
	};
}
