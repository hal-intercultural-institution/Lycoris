#include "render/animation.h"

#include <cmath>

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

void lycoris::render::animation::animation::set_frame(const float frame)
{
	frame_ = frame;
	const auto last_frame = keyframes_.back().get_frame();
	// index���Ō��1�O�������Ƃ��A�Ō�̃L�[�t�����I�[�o�[���Ă���Ƃ��́A�ŏ��ɖ߂��B
	if (last_frame <= frame_)
	{
		frame_ -= std::ceilf(frame_ / last_frame) * last_frame;
		current_index_ = 0;
		return;
	}

	const auto next_frame = keyframes_.at(current_index_ + 1).get_frame();
	// index + 1 �̃L�[�t���ɓ��B���Ă���΁Aindex++�B
	if (next_frame <= frame_) current_index_++;

}