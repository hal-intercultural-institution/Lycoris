#include "render/animation.h"

#include <cmath>

lycoris::render::animation::animation::animation(std::vector<keyframe>&& keyframes, std::string_view name)
{
	keyframes_ = std::move(keyframes);
	object_name_ = name;
}

const lycoris::render::animation::keyframe& lycoris::render::animation::animation::operator[](const std::size_t index) const
{
	return keyframes_[index];
}

const std::vector<lycoris::render::animation::keyframe>* lycoris::render::animation::animation::operator->() const
{
	return &keyframes_;
}

const lycoris::render::animation::keyframe& lycoris::render::animation::animation::at(const std::size_t index) const
{
	return keyframes_.at(index);
}

lycoris::render::animation::keyframe lycoris::render::animation::animation::interpolate()
{
	assert(keyframes_.size() > 1);

	const auto& keyframe1 = keyframes_.at(current_index_);
	const auto& keyframe2 = keyframes_.at(current_index_ + 1);
	const auto factor = (frame_ - keyframe1.get_frame()) / (keyframe2.get_frame() - keyframe1.get_frame());

	const auto position =
		DirectX::XMVectorLerp(XMLoadFloat3(&keyframe1.get_position()), XMLoadFloat3(&keyframe2.get_position()), factor);

	const auto rotation =
		DirectX::XMVectorLerp(XMLoadFloat3(&keyframe1.get_rotation()), XMLoadFloat3(&keyframe2.get_rotation()), factor);

	const auto scale =
		DirectX::XMVectorLerp(XMLoadFloat3(&keyframe1.get_scale()), XMLoadFloat3(&keyframe2.get_scale()), factor);

	DirectX::XMFLOAT3 position_f{}, rotation_f{}, scale_f{};

	XMStoreFloat3(&position_f, position);
	XMStoreFloat3(&rotation_f, rotation);
	XMStoreFloat3(&scale_f, scale);

	return keyframe(position_f, rotation_f, scale_f, frame_);
}

void lycoris::render::animation::animation::set_frame(const float frame)
{
	frame_ = frame;
	const auto last_frame = keyframes_.back().get_frame();
	// indexが最後の1つ前だったとき、最後のキーフレをオーバーしているときは、最初に戻す。
	if (last_frame <= frame_)
	{
		frame_ -= std::floorf(frame_ / last_frame) * last_frame;
		current_index_ = 0;
	}

	while (true)
	{
		const auto next_frame = keyframes_.at(current_index_ + 1).get_frame();
		// index + 1 のキーフレに到達していれば、index++。
		if (next_frame <= frame_) current_index_++;
		else break;
	}

}
