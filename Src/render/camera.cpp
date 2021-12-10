#include "render/camera.h"

#include "game.h"

lycoris::render::viewport::viewport(const float width, const float height, const float top_left_x, const float top_left_y)
{
	width_ = width;
	height_ = height;
	top_left_x_ = top_left_x;
	top_left_y_ = top_left_y;
}

D3D11_VIEWPORT lycoris::render::viewport::get() const noexcept
{
	const auto& screen = game::get_game().get_renderer().get_screen();

	return
	{
		screen.get_screen_width()* top_left_x_,
		screen.get_screen_height()* top_left_y_,
		screen.get_screen_width() * width_,
		screen.get_screen_height() * height_,
		0.f,
		1.f
	};
}

float lycoris::render::viewport::get_aspect_ratio() const noexcept
{
	const auto& screen = game::get_game().get_renderer().get_screen();
	return screen.get_screen_width() * width_ / screen.get_screen_height() * height_;
}

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

const lycoris::render::viewport& lycoris::render::camera::get_viewport() const noexcept
{
	return viewport_;
}

const DirectX::XMFLOAT4X4& lycoris::render::camera::get_inverted_view_matrix() const noexcept
{
	return inverted_view_matrix_;
}

void lycoris::render::camera::set_position(const DirectX::XMFLOAT3& position) noexcept
{
	position_ = position;
}

void lycoris::render::camera::set_rotation(const DirectX::XMFLOAT3& rotation) noexcept
{
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

void lycoris::render::camera::set_viewport(const viewport& viewport) noexcept
{
	viewport_ = viewport;
}

bool lycoris::render::camera::is_used() const noexcept
{
	return used_;
}

void lycoris::render::camera::set_use(const bool use) noexcept
{
	used_ = use;
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
	renderer.set_viewport(viewport_);
	
	// view matrix
	const auto view_matrix = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&position_), XMLoadFloat3(&looking_at_), XMLoadFloat3(&up_));
	XMStoreFloat4x4(&view_matrix_, view_matrix);
	renderer.set_view_matrix(view_matrix_);

	// view matrix (inverted)
	const auto inverted_view_matrix = XMMatrixInverse(nullptr, view_matrix);
	XMStoreFloat4x4(&inverted_view_matrix_, inverted_view_matrix);
	auto& ivm = inverted_view_matrix_;
	ivm._41 = ivm._42 = ivm._43 = 0.0f;

	// projection matrix
	const auto projection_matrix = DirectX::XMMatrixPerspectiveFovLH(1.0f, viewport_.get_aspect_ratio(), 1.0f, 10000.0f);
	XMStoreFloat4x4(&projection_matrix_, projection_matrix);
	renderer.set_projection_matrix(projection_matrix_);
}
