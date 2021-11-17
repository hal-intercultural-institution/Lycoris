﻿#pragma once

#include <vector>

#include "render/keyframe.h"

namespace lycoris::render::animation
{
	// animation data, per part
	class animation
	{
	public:
		animation() = default;
		~animation() = default;
		animation(const animation&) = delete;
		animation(animation&&) = default;

		animation& operator=(const animation&) = delete;
		animation& operator=(animation&&) = default;

		const keyframe& operator[](std::size_t index) const;
		const std::vector<keyframe>* operator->() const;

		const keyframe& at(std::size_t index) const;

		void set_frame(float frame);

	private:
		std::vector<keyframe> keyframes_{};
		std::size_t current_index_ = 0;
		float frame_ = 0.0f;
	};

	// animation controller
	class animator
	{
	public:
		animator() = default;
		~animator() = default;
		animator(const animator&) = delete;
		animator(animator&&) = default;

		animator& operator=(const animator&) = delete;
		animator& operator=(animator&&) = default;

		explicit operator bool() const;

		void set_frame(float frame);

	private:
		std::vector<animation> animations_{};
		float frame_ = 0.0f;
	};
}
