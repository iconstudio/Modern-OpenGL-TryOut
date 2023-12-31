module;
#include <Windows.h>
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLu.h>

module Glib;
import <type_traits>;
import <stdexcept>;
import :State;
import :ClearBits;
import Glib.Windows.Colour;

constinit static const GLubyte* version_string = nullptr;
constinit static const GLubyte* version_vendor = nullptr;
constinit static const GLubyte* version_render = nullptr;
constinit static const GLubyte* version_shader = nullptr;
constinit static const GLubyte* version_extent = nullptr;

std::string_view
gl::info::GetVersion()
noexcept
{
	version_string = glGetString(GL_VERSION);
	return reinterpret_cast<const char*>(version_string);
}

std::string_view
gl::info::GetVendor()
noexcept
{
	version_vendor = glGetString(GL_VENDOR);
	return reinterpret_cast<const char*>(version_vendor);
}

std::string_view
gl::info::GetRenderer()
noexcept
{
	version_render = glGetString(GL_RENDERER);
	return reinterpret_cast<const char*>(version_render);
}

std::string_view
gl::info::GetExtensions()
noexcept
{
	version_extent = glGetString(GL_EXTENSIONS);
	return reinterpret_cast<const char*>(version_extent);
}

std::string_view
gl::info::GetShadingLanguageVersion()
noexcept
{
	version_shader = glGetString(GL_SHADING_LANGUAGE_VERSION);
	return reinterpret_cast<const char*>(version_shader);
}

void
gl::global::SetState(const gl::State& state)
noexcept
{
	::glEnable(static_cast<GLenum>(state));
}

void
gl::global::SetState(gl::State&& state)
noexcept
{
	::glEnable(static_cast<GLenum>(state));
}

void
gl::global::SetState(const volatile gl::State& state)
noexcept
{
	::glEnable(static_cast<GLenum>(state));
}

void
gl::global::SetState(volatile gl::State&& state)
noexcept
{
	::glEnable(static_cast<GLenum>(state));
}

void
gl::global::SetState(const gl::State& state, bool flag)
noexcept
{
	if (flag)
	{
		::glEnable(static_cast<GLenum>(state));
	}
	else
	{
		::glDisable(static_cast<GLenum>(state));
	}
}

void
gl::global::SetState(const volatile gl::State& state, bool flag)
noexcept
{
	if (flag)
	{
		::glEnable(static_cast<GLenum>(state));
	}
	else
	{
		::glDisable(static_cast<GLenum>(state));
	}
}

void
gl::global::SetState(gl::State&& state, bool flag)
noexcept
{
	if (flag)
	{
		::glEnable(static_cast<GLenum>(state));
	}
	else
	{
		::glDisable(static_cast<GLenum>(state));
	}
}

void
gl::global::SetState(volatile gl::State&& state, bool flag)
noexcept
{
	if (flag)
	{
		::glEnable(static_cast<GLenum>(state));
	}
	else
	{
		::glDisable(static_cast<GLenum>(state));
	}
}

void
gl::global::SetBackgroundColour(const gl::win32::Colour& colour)
noexcept
{
	::glClearColor(colour.R / 255.0f, colour.G / 255.0f, colour.B / 255.0f, colour.A / 255.0f);
}

void
gl::global::SetBackgroundColour(gl::win32::Colour&& colour)
noexcept
{
	::glClearColor(colour.R / 255.0f, colour.G / 255.0f, colour.B / 255.0f, colour.A / 255.0f);
}

void
gl::global::SetBackgroundColour(const std::uint8_t& r, const std::uint8_t& g, const std::uint8_t& b, const std::uint8_t& a)
noexcept
{
	::glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

void
gl::global::Clear(Clearance target)
noexcept
{
	::glClear(static_cast<GLbitfield>(target));
}

void
gl::global::SetViewport(const std::int32_t& x, const std::int32_t& y, const std::uint32_t& width, const std::uint32_t& height)
noexcept
{
	::glViewport(x, y, width, height);
}

void
gl::global::SetViewport(std::int32_t&& x, std::int32_t&& y, std::uint32_t&& width, std::uint32_t&& height)
noexcept
{
	::glViewport(std::move(x), std::move(y), std::move(width), std::move(height));
}

bool
gl::global::IsBlending()
noexcept
{
	GLboolean result{};
	::glGetBooleanv(GL_BLEND, &result);

	return GL_TRUE == result;
}

bool
gl::global::IsCulling()
noexcept
{
	GLboolean result{};
	::glGetBooleanv(GL_CULL_FACE, &result);

	return GL_TRUE == result;
}

bool
gl::global::IsScissoring()
noexcept
{
	GLboolean result{};
	::glGetBooleanv(GL_SCISSOR_TEST, &result);

	return GL_TRUE == result;
}

bool
gl::global::IsTestingAlpha()
noexcept
{
	GLboolean result{};
	::glGetBooleanv(GL_ALPHA, &result);

	return GL_TRUE == result;
}

bool
gl::global::IsTestingDepth()
noexcept
{
	GLboolean result{};
	::glGetBooleanv(GL_DEPTH_TEST, &result);

	return GL_TRUE == result;
}

bool
gl::global::IsTestingStencil()
noexcept
{
	GLboolean result{};
	::glGetBooleanv(GL_STENCIL_TEST, &result);

	return GL_TRUE == result;
}
