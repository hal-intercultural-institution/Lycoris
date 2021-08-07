#pragma once

#include <utility>

namespace lycoris::utility
{
	template<typename T>
	class xaudio2_voice
	{
	public:
		xaudio2_voice() = default;
		
		xaudio2_voice(xaudio2_voice&& other) noexcept
		{
			if (ptr_)
			{
				ptr_->DestroyVoice();
			}
			ptr_ = other.ptr_;
			other.ptr_ = nullptr;
		}
		xaudio2_voice(const xaudio2_voice&) = delete;

		~xaudio2_voice() noexcept
		{
			if (ptr_)
			{
				ptr_->DestroyVoice();
				ptr_ = nullptr;
			}
		}

		xaudio2_voice& operator=(xaudio2_voice&& other) noexcept
		{
			if (ptr_)
			{
				ptr_->DestroyVoice();
			}
			ptr_ = other.ptr_;
			other.ptr_ = nullptr;
			return *this;
		}
		xaudio2_voice& operator=(const xaudio2_voice&) = delete;

		T& operator*()
		{
			return *ptr_;
		}

		T* operator->()
		{
			return ptr_;
		}
		
		T* get()
		{
			return ptr_;
		}

		T** put()
		{
			return &ptr_;
		}
	
	private:
		T* ptr_ = nullptr;
	};
}
