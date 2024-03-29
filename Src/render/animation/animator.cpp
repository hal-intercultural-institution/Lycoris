﻿#include "render/animation.h"

#include <stdexcept>

#include "utility/file.h"

lycoris::render::animation::animator::operator bool() const
{
	return !animations_.empty();
}

void lycoris::render::animation::animator::set_frame(const float frame)
{
	if (frame < 0.0f) throw std::invalid_argument("Animator: negative value is not allowed here");
	if (animations_.empty()) throw std::runtime_error("Animator: animation data are not loaded");
	frame_ = frame;

	assert(calculated_.size() == animations_.size());

	for (std::size_t i = 0; i < animations_.size(); ++i)
	{
		animations_.at(i).set_frame(frame);
		calculated_.at(i) = animations_.at(i).interpolate();
	}
}

void lycoris::render::animation::animator::increment(const float value)
{
	frame_ += value;
	if (frame_ < 0.0f) throw std::runtime_error("Animator: keyframe cannot be negative value");
	if (animations_.empty()) throw std::runtime_error("Animator: animation data are not loaded");

	assert(calculated_.size() == animations_.size());

	for (std::size_t i = 0; i < animations_.size(); ++i)
	{
		animations_.at(i).increment(value);
		calculated_.at(i) = animations_.at(i).interpolate();
	}
}

float lycoris::render::animation::animator::get_frame() const
{
	const auto last_frame = get_max_frame();
	return frame_ - std::floorf(frame_ / last_frame) * last_frame;
}

float lycoris::render::animation::animator::get_max_frame() const
{
	return animations_.at(0)->back().get_frame();
}

const std::vector<lycoris::render::animation::keyframe>& lycoris::render::animation::animator::get() const
{
	return calculated_;
}

const std::vector<lycoris::render::animation::keyframe>& lycoris::render::animation::animator::get(const float frame)
{
	set_frame(frame);
	return get();
}

lycoris::render::animation::animator lycoris::render::animation::animator::load_from_file(const std::filesystem::path& path)
{
	auto file = utility::file::text_file(path.string());

	std::vector<animation> animations;
	std::vector<std::string> str;

	std::string name;
	std::vector<keyframe> keyframes{};

	DirectX::XMFLOAT3 location{}, rotation{}, scale{};

	while (file.next_splitted_line(str))
	{
		if (str[0].starts_with("//") || str.size() < 2) continue;
		if (str[0] == "bindobj")
		{
			// 読み込みが初回ではない
			if (!animations.empty())
			{
				// 前回までの内容を保存してリセット
				keyframes.back() = keyframe(location, rotation, scale, keyframes.back().get_frame());
				animations.back() = animation(std::move(keyframes), name);
				keyframes = std::vector<keyframe>();
				name.clear();
			}
			animations.push_back({});
			name = str[1];
		}
		else if (str[0] == "definekey")
		{
			// キーフレ宣言が初回ではない
			if (!keyframes.empty())
			{
				// 読み取った値で更新
				keyframes.back() = keyframe(location, rotation, scale, keyframes.back().get_frame());
				location = rotation = scale = {};
			}
			// 適当な値とキーフレ値を入れて初期化
			keyframes.emplace_back(DirectX::XMFLOAT3{}, DirectX::XMFLOAT3{}, DirectX::XMFLOAT3{}, std::stof(str[1]));
		}
		else if (str[0] == "location")
		{
			location = { std::stof(str[1]), std::stof(str[2]), std::stof(str[3])};
		}
		else if (str[0] == "rotation")
		{
			rotation = { std::stof(str[1]), std::stof(str[2]), std::stof(str[3]) };
		}
		else if (str[0] == "scale")
		{
			scale = { std::stof(str[1]), std::stof(str[2]), std::stof(str[3]) };
		}
	}

	// 最終処理
	keyframes.back() = keyframe(location, rotation, scale, keyframes.back().get_frame());
	animations.back() = animation(std::move(keyframes), name);

	return animator(std::move(animations));
}

lycoris::render::animation::animator::animator(std::vector<animation>&& animations)
{
	animations_ = std::move(animations);
	calculated_ = std::vector<keyframe>(animations_.size());
}
