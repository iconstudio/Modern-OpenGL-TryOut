module;
#define NOMINMAX
#include <Windows.h>
#include "glew.h"
#include <gl\gl.h>
#include <gl\glu.h>

module Glib;
import <utility>;
import <memory>;
import <print>;
import Glib.Windows.Context;
import Glib.Windows.Context.Renderer;
import :System;
import :Blender;
import Glib.Culling;
import Glib.Legacy.Primitive;

static inline constexpr ::PIXELFORMATDESCRIPTOR opengl_format =
{
	sizeof(PIXELFORMATDESCRIPTOR),
	1,                     // version number
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SUPPORT_COMPOSITION | PFD_SWAP_EXCHANGE | PFD_GENERIC_ACCELERATED,
	PFD_TYPE_RGBA,         // RGBA type
	32,
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

static inline constexpr float background_coords[] =
{
	+1.0f, +1.0f, 0.0f,
	+1.0f, -1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	-1.0f, +1.0f, 0.0f,
};

GLuint background_color_buffer = 0;

void SinglePainter(gl::win32::IContext* const&) noexcept;
void DoublePainter(gl::win32::IContext* const&) noexcept;

unsigned long
_InitializePixelFormat(const gl::win32::IContext& hdc, PIXELFORMATDESCRIPTOR& my_format, int& my_target)
noexcept;

gl::System::System()
noexcept
	: base1(nullptr), base2()
{}

unsigned long
gl::System::Initialize(const gl::win32::IContext& hdc, const gl::system::Descriptor& descriptor)
noexcept
{
	mySettings = descriptor;

	::PIXELFORMATDESCRIPTOR my_format = opengl_format;
	int my_target = 0;

	if (mySettings.doubleBuffered)
	{
		my_format.dwFlags |= PFD_DOUBLEBUFFER;
	}

	if (unsigned long error = _InitializePixelFormat(hdc, my_format, my_target); 0 != error)
	{
		return error;
	}

	base1::operator=(hdc.Delegate(::wglCreateContext));
	if (nullptr == *this)
	{
		unsigned long error = ::GetLastError();
		std::println("Failed on creating a opengl context. (code: {})", error);
		return error;
	}

	if (0 != (my_format.dwFlags & PFD_DOUBLEBUFFER))
	{
		myPainter = DoublePainter;
	}
	else
	{
		myPainter = SinglePainter;
	}

	::wglMakeCurrent(hdc, GetHandle());

	GLenum err = ::glewInit();
	constinit static const GLubyte* error_string = nullptr;

	if (GLEW_OK != err)
	{
		error_string = ::glewGetErrorString(err);
		const char* temp_msg = reinterpret_cast<const char*>(error_string);

		std::println("Failed to initialize GLEW.");
		std::println("GLEW Error: {}", temp_msg);

		//util::Println("Error: {}", temp_msg));
		//util::Println(std::format("Error: {}", temp_msg));
		//util::Println(std::vformat("Error: {}", std::make_format_args(temp_msg)));
		//throw std::runtime_error("Failed to initialize GLEW");
		return ::GetLastError();
	}
	else if (::glewIsSupported("GL_VERSION_4_6"))
	{
		std::println("GL Version is 4.6");
	}

	// VBO 객체 생성
	::glGenBuffers(1, &background_color_buffer);
	// VBO 타입 바인딩
	::glBindBuffer(GL_ARRAY_BUFFER, background_color_buffer);
	// VBO에 위치 데이터 연결
	::glBufferData(GL_ARRAY_BUFFER, sizeof(background_coords), background_coords, GL_STATIC_DRAW);
	// 정점 속성 정의
	::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	::wglMakeCurrent(nullptr, nullptr);

	return _InitializeSystem();
}

HDC temp_context = nullptr;

bool
gl::System::BeginOpenGLContext(win32::IContext& ctx)
const noexcept
{
	if (0 == ctx.Delegate(::wglMakeCurrent, GetHandle()))
	{
		std::println("Failed to begin rendering. (gl error code: %u)\n", ::glGetError());

		return false;
	}

	//temp_context = ctx.GetHandle();
	return true;
}

bool
gl::System::BeginOpenGLContext(win32::IContext&& ctx)
const noexcept
{
	if (0 == ctx.Delegate(::wglMakeCurrent, GetHandle()))
	{
		std::println("Failed to begin rendering. (gl error code: %u)\n", ::glGetError());

		return false;
	}

	//temp_context = ctx.GetHandle();
	return true;
}

bool
gl::System::EndOpenGLContext()
const noexcept
{
	return 0 != ::wglMakeCurrent(nullptr, nullptr);
}

bool
gl::System::BeginRendering(gl::win32::IContext& painter)
noexcept
{
	using namespace gl::legacy;

	if (0 == painter.Delegate(::wglMakeCurrent, GetHandle()))
	{
		std::println("Failed to begin rendering. (gl error code: {})", ::glGetError());

		return false;
	}

	const int& view_x = ViewX();
	const int& view_y = ViewY();
	const int& view_w = ViewWidth();
	const int& view_h = ViewHeight();

	nativeContext = std::addressof(painter);

	myBlender->Apply();
	global::SetViewport(view_x, view_y, view_w, view_h);
	global::Clear(Clearance::DepthStencil);

	transform::SetMode(TransformMode::Projection);
	transform::PushState();

	global::SetBackgroundColour(mySettings.borderColour);
	global::Clear(Clearance::Color);

	transform::PushState();
	::glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	const auto& color = mySettings.backgroundColour;
	const float border = 1.01f;

	// VBO 타입 바인딩
	::glBindBuffer(GL_ARRAY_BUFFER, background_color_buffer);
	// 정점 속성 활성화
	::glEnableVertexAttribArray(0);
	//primitive::Begin(Primitive::Quads);
	//primitive::SetColour(color);
	//primitive::Vertex(-border, -border, 0.0f);
	//primitive::Vertex(-border, border, 0.0f);
	//primitive::Vertex(border, border, 0.0f);
	//primitive::Vertex(border, -border, 0.0f);
	//primitive::End();
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
	::glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

bool
gl::System::EndRendering()
noexcept
{
	transform::PopState();

	transform::SetMode(TransformMode::Projection);
	transform::PopState();

	myPainter(nativeContext);

	return 0 != ::wglMakeCurrent(nullptr, nullptr);
}

unsigned long
gl::System::_InitializeSystem()
noexcept
{
	myBlender = new gl::Blender{ gl::DefaultAlpha };

	if (mySettings.hiddenSurfaceRemoval)
	{
		global::SetState(gl::State::Culling);
		gl::Culling(gl::Face::Back);
		gl::CullingDirection(mySettings.cullingIsClockwise);
	}

	global::SetState(gl::State::Depth);

	aspectRatio = static_cast<double>(mySettings.viewPort.w) / static_cast<double>(mySettings.viewPort.h);

	if (mySettings.vSync)
	{
		typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALPROC)(int);
		PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

		//const char* extensions = (char*)glGetString(GL_EXTENSIONS);

		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");

		// no glew, so use old way
		if (wglSwapIntervalEXT)
		{
			wglSwapIntervalEXT(1);
		}
		//::wglSwapIntervalEXT(1);
	}

	return 0;
}

gl::System::~System()
noexcept
{
	if (nullptr != GetHandle())
	{
		Delegate(::wglDeleteContext);
	}

	delete myBlender;
}

std::shared_ptr<gl::System>
gl::CreateSystem()
noexcept
{
	return std::make_shared<gl::System>();
}

unsigned long
_InitializePixelFormat(const gl::win32::IContext& hdc, PIXELFORMATDESCRIPTOR& my_format, int& my_target)
noexcept
{
	const int target = ::ChoosePixelFormat(hdc, &my_format);
	if (0 == target)
	{
		unsigned long error = ::GetLastError();
		std::println("Failed on acquiring a pixel format. (code: {})", error);
		return error;
	}

	const int check = ::DescribePixelFormat(hdc, target, sizeof(my_format), &my_format);
	if (0 == check)
	{
		unsigned long error = ::GetLastError();
		std::println("Failed on reading a pixel format %d. (code: {})", target, error);
		return error;
	}

	if (0 == ::SetPixelFormat(hdc, target, &my_format))
	{
		unsigned long error = ::GetLastError();
		std::println("Failed on setting pixel format %d. (code: {})", target, error);
		return error;
	}

	if (0 == (my_format.dwFlags & PFD_SUPPORT_OPENGL))
	{
		unsigned long error = ::GetLastError();
		std::println("Failed on checking a pixel format %d. (code: {})", target, error);
		return error;
	}

	my_target = target;

	return 0UL;
}

void
SinglePainter(gl::win32::IContext* const&)
noexcept
{
	::glFlush();
}

void
DoublePainter(gl::win32::IContext* const& context)
noexcept
{
	context->Delegate(::SwapBuffers);
}

void
gl::System::KeepAspectRatio(bool keep_ratio)
noexcept
{
	if (mySettings.staticView) return;

	mySettings.keepAspectRatio = keep_ratio;
}

void gl::System::SetViewPosition(int x, int y) noexcept
{
	if (mySettings.staticView) return;

	mySettings.viewPort.x = x;
	mySettings.viewPort.y = y;
}

void
gl::System::SetViewSize(int width, int height)
noexcept
{
	if (mySettings.staticView) return;

	mySettings.viewPort.w = width;
	mySettings.viewPort.h = height;
}

void
gl::System::SetViewPort(const gl::Rect& size)
noexcept
{
	if (mySettings.staticView) return;

	mySettings.viewPort = size;
}

void
gl::System::SetViewPort(gl::Rect&& size)
noexcept
{
	if (mySettings.staticView) return;

	mySettings.viewPort = std::move(size);
}

void
gl::System::SetViewPort(int x, int y, int width, int height)
noexcept
{
	SetViewPort(gl::Rect{ x, y, width, height });
}

const gl::Rect&
gl::System::ViewPort()
const noexcept
{
	return mySettings.viewPort;
}

int&
gl::System::ViewX()
noexcept
{
	return mySettings.viewPort.x;
}

int&
gl::System::ViewY()
noexcept
{
	return mySettings.viewPort.y;
}

int&
gl::System::ViewWidth()
noexcept
{
	return mySettings.viewPort.w;
}

int&
gl::System::ViewHeight()
noexcept
{
	return mySettings.viewPort.h;
}

const int&
gl::System::ViewX()
const noexcept
{
	return mySettings.viewPort.x;
}

const int&
gl::System::ViewY()
const noexcept
{
	return mySettings.viewPort.y;
}

const int&
gl::System::ViewWidth()
const noexcept
{
	return mySettings.viewPort.w;
}

const int&
gl::System::ViewHeight()
const noexcept
{
	return mySettings.viewPort.h;
}

void
gl::System::UpdateViewPort(int client_hsize, int client_vsize)
noexcept
{
	clientWidth = client_hsize;
	clientHeight = std::max(1, client_vsize);

	if (mySettings.staticView) return;

	if (mySettings.keepAspectRatio)
	{
		const int clc_width = static_cast<int>(static_cast<double>(client_vsize) * aspectRatio);
		const int new_hs = std::min(clc_width, client_hsize);
		const int new_vs = static_cast<int>(static_cast<double>(new_hs) / aspectRatio);

		const int new_x = (client_hsize - new_hs) / 2;
		const int new_y = (client_vsize - new_vs) / 2;
		SetViewPort(new_x, new_y, new_hs, new_vs);
	}
	else
	{
		SetViewPort(0, 0, client_hsize, client_vsize);
	}
}

double
gl::System::AspectRatio()
const noexcept
{
	return aspectRatio;
}
