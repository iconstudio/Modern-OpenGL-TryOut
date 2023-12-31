export module Glib.Windows.Resource.Brush;
import <type_traits>;
import Glib.Windows.Definitions;
import Glib.Windows.IGraphics;
import Glib.Windows.Colour;

export namespace gl::win32::resource
{
	class [[nodiscard]] ColorBrush : public IGraphics<ColorBrush, native::RawColorBrush>
	{
	public:
		using base = IGraphics<ColorBrush, native::RawColorBrush>;

		constexpr ColorBrush(nullptr_t) noexcept
			: base(nullptr)
		{}

		explicit constexpr ColorBrush(handle_type&& brush) noexcept
			: base(std::move(brush))
		{}

		~ColorBrush() noexcept;

		bool Destroy() noexcept;

		ColorBrush& operator=(nullptr_t) noexcept;
		ColorBrush& operator=(handle_type&& brush) noexcept;
		ColorBrush& operator=(ColorBrush&& other) noexcept;

		ColorBrush(const ColorBrush&) = delete;
		constexpr ColorBrush(ColorBrush&&) noexcept = default;
		ColorBrush& operator=(const ColorBrush&) = delete;
	};

	enum class [[nodiscard]] BrushPatterns
	{
		Horizontal = 0, // HS_HORIZONTAL
		Vertical = 1, // HS_VERTICAL
		DownDiagonal = 2, // HS_FDIAGONAL
		UpDiagonal = 3, // HS_BDIAGONAL
		Cross = 4, // HS_CROSS,
		CrossDiagonal = 5, // HS_DIAGCROSS
	};
}

export namespace gl::win32
{
	[[nodiscard]]
	resource::ColorBrush MakeColorBrush(const unsigned long& color) noexcept;

	[[nodiscard]]
	resource::ColorBrush MakeColorBrush(unsigned long&& color) noexcept;

	[[nodiscard]]
	resource::ColorBrush MakeColorBrush(const Colour& color) noexcept;

	[[nodiscard]]
	resource::ColorBrush MakeColorBrush(Colour&& color) noexcept;

	[[nodiscard]]
	resource::ColorBrush MakePatternBrush(const resource::BrushPatterns& pattern, const unsigned long& rgb) noexcept;

	[[nodiscard]]
	resource::ColorBrush MakePatternBrush(const resource::BrushPatterns& pattern, const Colour& argb) noexcept;

	[[nodiscard]]
	resource::ColorBrush MakeStampBrush(const native::RawBitmap& image) noexcept;
}
