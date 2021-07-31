//=============================================================================
//
// ÉJÉÅÉâèàóù [camera.cpp]
// Author : 
//
//=============================================================================
#include "render/camera.h"

#include "game.h"
#include "gamedef.h"

void lycoris::render::camera::initialize()
{
	looking_at_ = { 0.0f, 0.0f, 0.0 };
	up_ = { 0.0f, 1.0f, 0.0f };
	position_ = { 0.0f, 25.0f, 60.0f };
	rotation_ = { 0.0f, 0.0f, 0.0f, 0.0f };
}

void lycoris::render::camera::on_tick() { }

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
