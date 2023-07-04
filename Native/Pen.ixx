module;
#include "Internal.hpp"

export module Glib.Device.Pen;
import <type_traits>;
import Glib.Device.Definitions;
import Glib.Device.IHandle;
import Glib.Device.Colour;

export namespace gl::device
{
	enum class [[nodiscard]] PenStyles
	{
		None = 5,// PS_NULL
		Fill = 0, // PS_SOLID
		Dash = 1, // PS_DASH
		Dots = 2, // PS_DOT
		DashDot = 3, // PS_DASHDOT
		//Frame = 6, // PS_INSIDEFRAME
	};

	class [[nodiscard]] Pen : public IHandle<native::RawPen>
	{
		constexpr Pen() noexcept = default;

	public:
		using base = IHandle<native::RawPen>;

		Pen(const PenStyles& style, const int& width, const Colour& color) noexcept;
		Pen(const PenStyles& style, const int& width, Colour&& color) noexcept;
		Pen(const PenStyles& style, const int& width, const RawRGB& color) noexcept;
		Pen(const PenStyles& style, const int& width, RawRGB&& color) noexcept;
		~Pen() noexcept;

		void Destroy() noexcept;
		Pen& operator=(nullptr_t) noexcept;

		Pen(const Pen&) = delete;
		Pen(Pen&&) = delete;
		Pen& operator=(const Pen&&) = delete;
		Pen& operator=(Pen&&) = delete;

	private:
		PenStyles myStyle;
		Colour myColor;
		int mySize;
	};
}
