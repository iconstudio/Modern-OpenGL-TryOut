module;
#include <Windows.h>

export module Glib.Display;
import Glib.Rect;

export namespace gl::display
{
	enum class [[nodiscard]] DpiAwareness
	{
		Invalid = ::DPI_AWARENESS_INVALID,
		Unaware = ::DPI_AWARENESS_UNAWARE,
		System = ::DPI_AWARENESS_SYSTEM_AWARE,
		Aware = ::DPI_AWARENESS_PER_MONITOR_AWARE
	};

	[[nodiscard]] Rect GetActualDisplaySize() noexcept;
	[[nodiscard]] Rect GetDisplaySize() noexcept;
	[[nodiscard]] bool IsDimmingMode();

	namespace dpi
	{
		constexpr UINT DEFAULT_DPI = 96U;

		bool SetDPIAware(const bool& enable) noexcept;
		[[nodiscard]] bool IsDPIAware() noexcept;
		[[nodiscard]] DpiAwareness GetDPIAware() noexcept;
		[[nodiscard]] bool IsDPIAware(const HWND& handle) noexcept;
		[[nodiscard]] consteval unsigned int GetDefaultDPI() noexcept { return DEFAULT_DPI; }
		[[nodiscard]] DpiAwareness GetDPIAware(const HWND& handle) noexcept;
		[[nodiscard]] unsigned int GetDPI() noexcept;
	}
}
