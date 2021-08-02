#pragma once

#include <winrt/base.h>
#include <d3d11.h>

#include "gamedef.h"

namespace lycoris::render
{
	template <typename T, std::uint32_t Slot>
	class constant_buffer
	{
	public:
		constant_buffer() = default;
		explicit constant_buffer(winrt::com_ptr<ID3D11Buffer>&& other_ptr)
		{
			buffer_ptr_ = std::move(other_ptr);
		}
		// restrict the use of copy constructor
		constant_buffer(const constant_buffer<T, Slot>&) = delete;
		constant_buffer(constant_buffer<T, Slot>&&) = default;
		~constant_buffer() = default;

		static constant_buffer<T, Slot> create()
		{
			winrt::com_ptr<ID3D11Buffer> buffer;

			D3D11_BUFFER_DESC buffer_desc;
			buffer_desc.ByteWidth = sizeof(T);
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = sizeof(float);

			auto& renderer = game::get_game().get_renderer();
			renderer.get_device().CreateBuffer(&buffer_desc, nullptr, buffer.put());

			std::array buffers = {
				buffer.get()
			};
			renderer.get_device_context().VSSetConstantBuffers(Slot, static_cast<std::uint32_t>(buffers.size()), buffers.data());

			return constant_buffer<T, Slot>(std::move(buffer));
		}

		void update()
		{
			auto& renderer = game::get_game().get_renderer();
			renderer.get_device_context().UpdateSubresource(buffer_ptr_.get(), 0, nullptr, &data_, 0, 0);
		}

		void update(T data)
		{
			data_ = data;
			auto& renderer = game::get_game().get_renderer();
			renderer.get_device_context().UpdateSubresource(buffer_ptr_.get(), 0, nullptr, &data_, 0, 0);
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
		constant_buffer<T, Slot>& operator=(const constant_buffer<T, Slot>&) = delete;
		constant_buffer<T, Slot>& operator=(constant_buffer<T, Slot>&&) = default;

	private:
		T data_;
		winrt::com_ptr<ID3D11Buffer> buffer_ptr_;
	};
}