#pragma once

#include <utility>

namespace lycoris::utility
{
	template<typename T>
	class xaudio2_voice
	{
	public:
		xaudio2_voice() = default;
		
		xaudio2_voice(xaudio2_voice&&) = default;
		xaudio2_voice(const xaudio2_voice&) = delete;

		~xaudio2_voice()
		{
			destroy();
		}

		xaudio2_voice& operator=(xaudio2_voice&&) = default;
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
		void destroy()
		{
			std::exchange(ptr_, nullptr)->DestroyVoice();
		}
		
		T* ptr_ = nullptr;
	};
}
