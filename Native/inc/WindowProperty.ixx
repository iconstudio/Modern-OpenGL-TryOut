module;
#include "Internal.hpp"

export module Glib.Windows.Client.Property;
import <type_traits>;
import Glib.Windows.Definitions;
import Glib.Windows.ProcessInstance;

export namespace gl::win32
{
	using native::WindowProcedure;
	using native::RawWindowProperty;

	inline constexpr RawWindowProperty DefaultWindowProperty = 
	{
		.cbSize = sizeof(RawWindowProperty),
			.style = CS_HREDRAW | CS_VREDRAW, // | CS_OWNDC,
			.lpfnWndProc = nullptr,
			.cbClsExtra = 0,
			.cbWndExtra = sizeof(void*),
			.hInstance = nullptr,
			.hIcon = nullptr,
			.hCursor = nullptr,
			.hbrBackground = nullptr,
			.lpszMenuName = nullptr,
			.lpszClassName = nullptr,
			.hIconSm = nullptr,
	};

	class [[nodiscard]] WindowProperty
	{
	public:
		constexpr WindowProperty() noexcept = default;
		constexpr ~WindowProperty() noexcept = default;

		explicit constexpr WindowProperty(const RawWindowProperty& data) noexcept
			: myWindowClass(data)
		{}

		explicit constexpr WindowProperty(RawWindowProperty&& data) noexcept
			: myWindowClass(std::move(data))
		{}

		inline bool Register() noexcept
		{
			return FALSE != ::RegisterClassEx(std::addressof(myWindowClass));
		}

		inline bool Deregister() noexcept
		{
			return FALSE != ::UnregisterClass(myWindowClass.lpszClassName, myWindowClass.hInstance);
		}

		[[nodiscard]]
		constexpr const RawWindowProperty& GetHandle() const& noexcept
		{
			return myWindowClass;
		}

		[[nodiscard]]
		constexpr RawWindowProperty&& GetHandle() && noexcept
		{
			return static_cast<RawWindowProperty&&>(myWindowClass);
		}

		[[nodiscard]]
		constexpr ProcessInstance GetInstance() const& noexcept
		{
			return ProcessInstance{ myWindowClass.hInstance };
		}

		[[nodiscard]]
		constexpr ProcessInstance GetInstance() && noexcept
		{
			return ProcessInstance{ static_cast<HINSTANCE&&>(myWindowClass.hInstance) };
		}

		[[nodiscard]]
		constexpr const WindowProcedure& GetProcedure() const& noexcept
		{
			return myWindowClass.lpfnWndProc;
		}

		[[nodiscard]]
		constexpr WindowProcedure&& GetProcedure() && noexcept
		{
			return static_cast<WindowProcedure&&>(myWindowClass.lpfnWndProc);
		}

		[[nodiscard]]
		constexpr const wchar_t* const& GetClass() const& noexcept
		{
			return myWindowClass.lpszClassName;
		}

		[[nodiscard]]
		constexpr const wchar_t*&& GetClass() && noexcept
		{
			return static_cast<const wchar_t*&&>(myWindowClass.lpszClassName);
		}

		constexpr WindowProperty(const WindowProperty&) noexcept = default;
		constexpr WindowProperty& operator=(const WindowProperty&) noexcept = default;
		constexpr WindowProperty(WindowProperty&&) noexcept = default;
		constexpr WindowProperty& operator=(WindowProperty&&) noexcept = default;

		RawWindowProperty myWindowClass{};
	};
}
