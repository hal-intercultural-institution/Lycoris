﻿#pragma once

#include <DirectXMath.h>

namespace lycoris::render::animation
{
	class keyframe
	{
	public:
		keyframe(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale, const float frame)
			: position_(position), rotation_(rotation), scale_(scale), frame_(frame) {}

		const DirectX::XMFLOAT3& get_position() const
		{
			return position_;
		}

		const DirectX::XMFLOAT3& get_rotation() const
		{
			return rotation_;
		}

		const DirectX::XMFLOAT3& get_scale() const
		{
			return scale_;
		}

		float get_frame() const
		{
			return frame_;
		}

	private:
		DirectX::XMFLOAT3 position_, rotation_, scale_;
		float frame_;
	};

}
