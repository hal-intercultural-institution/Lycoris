#pragma once

#include <winrt/base.h>
#include <d2d1.h>
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

	class text_color
	{
	public:
		text_color() = default;
		explicit text_color(winrt::com_ptr<ID2D1SolidColorBrush>&& ptr);

		const ID2D1SolidColorBrush& get() const;

	private:
		winrt::com_ptr<ID2D1SolidColorBrush> brush_;
	};

	class text_canvas
	{
	public:
		text_canvas() = default;
		text_canvas(float left, float top, float right, float bottom);

		const D2D1_RECT_F& get() const;
		D2D1_RECT_F& get();

	private:
		D2D1_RECT_F rectangle_{};
	};

}
