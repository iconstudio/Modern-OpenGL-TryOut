module;
#include <Windows.h>

module Glib.GraphicContext;
import <cstdio>;
import Glib.Device.Context.Renderer;

static inline constexpr ::PIXELFORMATDESCRIPTOR opengl_format =
{
	sizeof(PIXELFORMATDESCRIPTOR),
	1,                     // version number
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION | PFD_SWAP_EXCHANGE | PFD_GENERIC_ACCELERATED,
	PFD_TYPE_RGBA,         // RGBA type
	24,
	0, 0, 0, 0, 0, 0,      // color bits ignored
	8,
	0,                     // shift bit ignored
	0,                     // no accumulation buffer
	0, 0, 0, 0,            // accum bits ignored
	16,
	8,
	0,                     // no auxiliary buffer
	PFD_MAIN_PLANE,        // main layer
	0,                     // reserved
	0, 0, 0                // layer masks ignored
};

gl::GLContext::GLContext()
noexcept
	: base(nullptr)
{}

gl::GLContext::~GLContext()
noexcept
{
	if (nullptr != GetHandle())
	{
		::wglDeleteContext(GetHandle());
	}
}

unsigned long
gl::GLContext::Initialize(
	const gl::device::IContext& hdc
	, int view_width
	, int view_height
) noexcept
{
	auto& handle = GetHandle();

	const int target = ChoosePixelFormat(hdc, &opengl_format);
	if (0 == target)
	{
		unsigned long error = ::GetLastError();
		std::printf("Failed on acquiring a pixel format. (code: %u)\n", error);
		return error;
	}

	PIXELFORMATDESCRIPTOR checker{};

	const int check = DescribePixelFormat(hdc, target, sizeof(checker), &checker);
	if (0 == check)
	{
		unsigned long error = ::GetLastError();
		std::printf("Failed on reading a pixel format %d. (code: %u)\n", target, error);
		return error;
	}

	if (0 == (checker.dwFlags & PFD_SUPPORT_OPENGL))
	{
		unsigned long error = ::GetLastError();
		std::printf("Failed on checking a pixel format %d. (code: %u)\n", target, error);
		return error;
	}

	if (0 == SetPixelFormat(hdc, target, &checker))
	{
		unsigned long error = ::GetLastError();
		std::printf("Failed on setting pixel format %d. (code: %u)\n", target, error);
		return error;
	}

	handle = ::wglCreateContext(hdc);
	if (nullptr == handle)
	{
		unsigned long error = ::GetLastError();
		std::printf("Failed on creating a opengl context. (code: %u)\n", error);
		return error;
	}

	return 0;
}

bool gl::GLContext::Begin(device::GraphicDeviceContext& painter) noexcept
{
	auto& handle = GetHandle();

	if (0 == ::wglMakeCurrent(painter, handle))
	{
		std::printf("Failed on selecting a pixel format. (code: %u)\n", ::GetLastError());

		return false;
	}

	return true;
}

bool gl::GLContext::End() noexcept
{
	//::wglMakeCurrent(GetHandle(), nullptr);
	return 0 != ::wglMakeCurrent(nullptr, nullptr);
}
