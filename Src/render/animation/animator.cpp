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
