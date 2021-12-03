#pragma once

#include <winrt/base.h>
#include <dwrite.h>

namespace lycoris::render
{
	class text_format
	{
	public:
		text_format() = default;
		explicit text_format(winrt::com_ptr<IDWriteTextFormat>&& ptr);

		const IDWriteTextFormat& get() const;

	private:
		winrt::com_ptr<IDWriteTextFormat> d_write_text_format_;
	};

}
