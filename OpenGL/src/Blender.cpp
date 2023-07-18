module;
#include <Windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

module Glib.Blender;
import <utility>;
import Glib;

gl::Blender::Blender(bool enable)
noexcept
	: isBlending(enable)
	, wasBlending(global::IsBlending())
{
	if (isBlending && !wasBlending)
	{
		global::SetState(gl::State::Blending, true);
	}
	else if (!isBlending && wasBlending)
	{
		global::SetState(gl::State::Blending, false);
	}
}

gl::Blender::Blender(BlendOption src, BlendOption dest)
noexcept
	: isBlending(true)
	, wasBlending(global::IsBlending())
{
	GLint prev_source = 0;
	::glGetIntegerv(GL_BLEND_SRC, &prev_source);
	prevSrcMode = static_cast<BlendOption>(prev_source);

	GLint prev_dest = 0;
	::glGetIntegerv(GL_BLEND_DST, &prev_dest);
	prevDstMode = static_cast<BlendOption>(prev_dest);

	::glBlendFunc(static_cast<GLenum>(src), static_cast<GLenum>(dest));
	mySrcMode = src;
	myDstMode = dest;

	if (!wasBlending)
	{
		global::SetState(gl::State::Blending, true);
	}
}

gl::Blender::~Blender()
noexcept
{
	if (isBlending && !wasBlending)
	{
		global::SetState(gl::State::Blending, false);
	}
	else if (!isBlending && wasBlending)
	{
		global::SetState(gl::State::Blending, true);
	}

	if (BlendOption::None != prevDstMode)
	{
		::glBlendFunc(static_cast<GLenum>(prevSrcMode), static_cast<GLenum>(prevDstMode));
	}
}

bool
gl::Blender::IsBlending()
const noexcept
{
	return isBlending;
}

util::Monad<gl::BlendOption>
gl::Blender::GetSrcMode()
const noexcept
{
	if (BlendOption::None != mySrcMode)
	{
		return mySrcMode;
	}
	else
	{
		return util::nullopt;
	}
}

util::Monad<gl::BlendOption>
gl::Blender::GetDstMode()
const noexcept
{
	if (BlendOption::None != myDstMode)
	{
		return myDstMode;
	}
	else
	{
		return util::nullopt;
	}
}

util::Monad<gl::BlendOption>
gl::Blender::GetPrevSrcMode()
const noexcept
{
	if (BlendOption::None != prevSrcMode)
	{
		return prevSrcMode;
	}
	else
	{
		return util::nullopt;
	}
}

util::Monad<gl::BlendOption>
gl::Blender::GetPrevDstMode()
const noexcept
{
	if (BlendOption::None != prevDstMode)
	{
		return prevDstMode;
	}
	else
	{
		return util::nullopt;
	}
}

void
gl::Blender::swap(gl::Blender& other)
noexcept
{
	std::swap(isBlending, other.isBlending);
	std::swap(wasBlending, other.wasBlending);
	std::swap(mySrcMode, other.mySrcMode);
	std::swap(myDstMode, other.myDstMode);
	std::swap(prevSrcMode, other.prevSrcMode);
	std::swap(prevDstMode, other.prevDstMode);
}
