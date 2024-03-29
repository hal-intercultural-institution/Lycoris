﻿#pragma once

#include <filesystem>
#include <vector>

#include "render/keyframe.h"

namespace lycoris::render::animation
{
	// animation data, per part
	class animation
	{
	public:
		animation() = default;
		explicit animation(std::vector<keyframe>&& keyframes, std::string_view name);
		~animation() = default;
		animation(const animation&) = delete;
		animation(animation&&) = default;

		animation& operator=(const animation&) = delete;
		animation& operator=(animation&&) = default;

		const keyframe& operator[](std::size_t index) const;
		const std::vector<keyframe>* operator->() const;

		const keyframe& at(std::size_t index) const;
		keyframe interpolate();

		void set_frame(float frame);
		void increment(float value);

	private:
		std::vector<keyframe> keyframes_{};
		std::size_t current_index_ = 0;
		std::string object_name_;
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

		// sets frame and interpolate keyframes
		void set_frame(float frame);
		// increment frame
		void increment(float value);
		// gets current frame
		float get_frame() const;
		// gets maximum frame
		float get_max_frame() const;
		// gets calculated keyframes
		const std::vector<keyframe>& get() const;
		// set_frame(float) & get()
		const std::vector<keyframe>& get(float frame);

		[[nodiscard]] static animator load_from_file(const std::filesystem::path& path);

	private:
		explicit animator(std::vector<animation>&& animations);
		std::vector<animation> animations_{};
		std::vector<keyframe> calculated_{0};
		float frame_ = 0.0f;
	};
}
