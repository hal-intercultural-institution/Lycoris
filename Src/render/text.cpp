#include "render/text.h"

lycoris::render::text_format::text_format(winrt::com_ptr<IDWriteTextFormat>&& ptr)
{
	d_write_text_format_ = std::move(ptr);
}

const IDWriteTextFormat& lycoris::render::text_format::get() const
{
	assert(d_write_text_format_);
	return *d_write_text_format_.get();
}

lycoris::render::text_color::text_color(winrt::com_ptr<ID2D1SolidColorBrush>&& ptr)
{
	brush_ = std::move(ptr);
}

const ID2D1SolidColorBrush& lycoris::render::text_color::get() const
{
	assert(brush_);
	return *brush_.get();
}

lycoris::render::text_canvas::text_canvas(const float left, const float top, const float right, const float bottom)
{
	rectangle_.left = left;
	rectangle_.top = top;
	rectangle_.right = right;
	rectangle_.bottom = bottom;
}

const D2D1_RECT_F& lycoris::render::text_canvas::get() const
{
	return rectangle_;
}

D2D1_RECT_F& lycoris::render::text_canvas::get()
{
	return rectangle_;
}
