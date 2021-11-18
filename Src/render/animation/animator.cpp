#include "render/animation.h"

#include <stdexcept>

lycoris::render::animation::animator::operator bool() const
{
	return !animations_.empty();
}

void lycoris::render::animation::animator::set_frame(const float frame)
{
	if (frame >= 0.0f) throw std::invalid_argument("Animator: negative value is not allowed here");
	if (animations_.empty()) throw std::runtime_error("Animator: animation data are not loaded");
	frame_ = frame;

	for (auto& animation : animations_)
	{
		// キーフレが1つ以下なものはアニメーションではないので何もしない
		if (animation->size() <= 1) continue;
		animation.set_frame(frame);
	}
}

const std::vector<lycoris::render::animation::keyframe>& lycoris::render::animation::animator::get()
{
	assert(calculated_.size() == animations_.size(), "something went wrong about calculated vector size");

	for (std::size_t i = 0; i < animations_.size(); ++i)
	{
		calculated_.at(i) = animations_.at(i).interpolate();
	}

	return calculated_;
}

const std::vector<lycoris::render::animation::keyframe>& lycoris::render::animation::animator::get(const float frame)
{
	set_frame(frame);
	return get();
}
