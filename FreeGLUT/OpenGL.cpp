module;
module Glib;
import Glib.Rect;

inline static int window_handle = -1;
inline static gl::DisplayModes window_mode{};
inline static gl::Rect window_rect{};

void gl::Initialize(const DisplayModes& mode, const int& x, const int& y, const int& w, const int& h) noexcept
{
	window_rect.x = x;
	window_rect.y = y;
	window_rect.w = w;
	window_rect.h = h;
	window_mode = mode;
}

void gl::Initialize(const DisplayModes& mode, default_position_t, const int& w, const int& h) noexcept
{
	return Initialize(mode, default_window_x, default_window_y, w, h);
}

void gl::Initialize(const DisplayModes& mode, const int& x, const int& y, default_resoulution_t) noexcept
{
	return Initialize(mode, x, y, default_window_w, default_window_h);
}

void gl::Initialize(const DisplayModes& mode, const int& w, const int& h) noexcept
{
	return Initialize(mode, default_window_x, default_window_y, w, h);
}

void gl::Initialize(const DisplayModes& mode, default_position_t, default_resoulution_t) noexcept
{
	return Initialize(mode, default_window_x, default_window_y, default_window_w, default_window_h);
}