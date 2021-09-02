//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : 
//
//=============================================================================
#include "render/camera.h"

#include "game.h"
#include "gamedef.h"

const DirectX::XMFLOAT3& lycoris::render::camera::get_position() const noexcept
{
	return position_;
}

const DirectX::XMFLOAT3& lycoris::render::camera::get_looking_at() const noexcept
{
	return looking_at_;
}

const DirectX::XMFLOAT3& lycoris::render::camera::get_rotation() const noexcept
{
	return rotation_;
}

float lycoris::render::camera::get_camera_distance() const noexcept
{
	return distance_;
}

void lycoris::render::camera::set_position(const DirectX::XMFLOAT3& position) noexcept
{
	position_ = position;
}

//void lycoris::render::camera::set_rotation(const DirectX::XMFLOAT4& rotation) noexcept
//{
//	rotation_ = rotation;
//}

void lycoris::render::camera::set_rotation(const DirectX::XMFLOAT3& rotation) noexcept
{
	//XMStoreFloat4(&rotation_, DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
	rotation_ = rotation;
}

void lycoris::render::camera::set_looking_at(const DirectX::XMFLOAT3& position) noexcept
{
	looking_at_ = position;
}

void lycoris::render::camera::set_distance(float distance) noexcept
{
	distance_ = distance;
}

void lycoris::render::camera::initialize()
{
	looking_at_ = { 0.0f, 0.0f, 0.0f };
	up_ = { 0.0f, 1.0f, 0.0f };
	position_ = { 0.0f, 0.0f, 0.0f };
	rotation_ = { 0.0f, 0.0f, 0.0f };
}

void lycoris::render::camera::on_tick()
{
	if (rotation_.x > DirectX::XM_PI) rotation_.x = DirectX::XM_PI;
	if (rotation_.x < 0.001f) rotation_.x = 0.001f;
	const float x = distance_ * std::sinf(rotation_.x) * std::cosf(rotation_.y);
	const float y = distance_ * std::cosf(rotation_.x);
	const float z = distance_ * std::sinf(rotation_.x) * std::sinf(rotation_.y);
	position_ = DirectX::XMFLOAT3(looking_at_.x + x, looking_at_.y + y, looking_at_.z + z);
}

void lycoris::render::camera::set()
{
	auto& renderer = game::get_game().get_renderer();

	// view matrix
	const auto view_matrix = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&position_), XMLoadFloat3(&looking_at_), XMLoadFloat3(&up_));
	XMStoreFloat4x4(&view_matrix_, view_matrix);
	renderer.set_view_matrix(view_matrix_);

	// view matrix (inverted)
	const auto inverted_view_matrix = XMMatrixInverse(nullptr, view_matrix);
	XMStoreFloat4x4(&inverted_view_matrix_, inverted_view_matrix);

	// projection matrix
	const auto aspect_ratio = renderer.get_screen().get_screen_width() / renderer.get_screen().get_screen_height();
	const auto projection_matrix = DirectX::XMMatrixPerspectiveFovLH(1.0f, aspect_ratio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&projection_matrix_, projection_matrix);
	renderer.set_projection_matrix(projection_matrix_);
}
