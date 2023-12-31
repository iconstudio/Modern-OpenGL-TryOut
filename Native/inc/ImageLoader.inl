#pragma once

#ifndef _ICONER_IMAGE_LOADER_
#define _ICONER_IMAGE_LOADER_
#include "Internal.hpp"
import Glib.Windows.Definitions;
import Utility.Monad.Loosen;

using gl::win32::FilePath;

inline constexpr unsigned int _DefaultFlag = LR_SHARED | LR_LOADFROMFILE | LR_DEFAULTSIZE;

util::LooseMonad<HBITMAP, unsigned long>
_LoadBitmap(const FilePath& path)
noexcept
{
	HANDLE handle = ::LoadImage(nullptr, path.c_str(), IMAGE_BITMAP, 0, 0, _DefaultFlag);
	if (nullptr == handle)
	{
		return ::GetLastError();
	}
	else
	{
		return static_cast<HBITMAP>(handle);
	}
}

util::LooseMonad<HBITMAP, unsigned long>
_LoadResourceBitmap(const wchar_t* const& name)
noexcept
{
	HBITMAP result = ::LoadBitmap(nullptr, name);
	if (nullptr == result)
	{
		return ::GetLastError();
	}
	else
	{
		return result;
	}
}

util::LooseMonad<HICON, unsigned long>
_LoadIcon(const FilePath& path)
noexcept
{
	HANDLE handle = ::LoadImage(nullptr, path.c_str(), IMAGE_ICON, 0, 0, _DefaultFlag);
	if (nullptr == handle)
	{
		return ::GetLastError();
	}
	else
	{
		return static_cast<HICON>(handle);
	}
}

util::LooseMonad<HCURSOR, unsigned long>
_LoadCursor(const FilePath& path)
noexcept
{
	HANDLE handle = ::LoadImage(nullptr, path.c_str(), IMAGE_CURSOR, 0, 0, _DefaultFlag);
	if (nullptr == handle)
	{
		return ::GetLastError();
	}
	else
	{
		return static_cast<HCURSOR>(handle);
	}
}
#endif // _ICONER_IMAGE_LOADER_
