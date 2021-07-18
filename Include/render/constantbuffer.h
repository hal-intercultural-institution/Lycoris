#pragma once

#include <stdexcept>

#include <winrt/base.h>

#include "render/renderer.h"
#include "game.h"

namespace lycoris::render
{
	template <typename T, int slot>
	class constant_buffer
	{
		static_assert(slot >= 0, "slot of constant buffer is must be greater than 0");
	public:
		constant_buffer() = default;
		// restrict the use of copy constructor
		constant_buffer(const constant_buffer<T, slot>&) = delete;

		void create()
		{
			if (is_created_) throw std::runtime_error("ConstantBuffer: cbuff is already created");
			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.ByteWidth = sizeof(T);
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = sizeof(float);

			auto& renderer = game::get_game().get_renderer();

			renderer.get_device().CreateBuffer(&bufferDesc, nullptr, buffer_ptr_.put());
			renderer.get_device_context().VSSetConstantBuffers(slot, 1, buffer_ptr_.put());
			renderer.get_device_context().PSSetConstantBuffers(slot, 1, buffer_ptr_.put());
			is_created_ = true;
		}

		void update()
		{
			if (!is_created_) throw std::runtime_error("ConstantBuffer: failed to update cbuffer (not created yet)");
		}

		T& get()
		{
			return data_;
		}

		T* operator->()
		{
			return &data_;
		}

		// restrict the use of equal operator
		constant_buffer<T, slot>& operator=(const constant_buffer<T, slot>&) = delete;

	private:
		T data_;
		bool is_created_ = false;
		winrt::com_ptr<ID3D11Buffer> buffer_ptr_;
	};
}
