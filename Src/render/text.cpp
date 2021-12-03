﻿#include "render/text.h"

lycoris::render::text_format::text_format(winrt::com_ptr<IDWriteTextFormat>&& ptr)
{
	d_write_text_format_ = std::move(ptr);
}

const IDWriteTextFormat& lycoris::render::text_format::get() const
{
	assert(d_write_text_format_);
	return *d_write_text_format_.get();
}
