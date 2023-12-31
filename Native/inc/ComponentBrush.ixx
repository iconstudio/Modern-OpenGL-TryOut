export module Glib.Windows.Resource.Brush.Component;
import Glib.Windows.Definitions;

export namespace gl::win32
{
	extern "C" enum class [[nodiscard]] ColoredComponent : int
	{
		Window = 5, // COLOR_WINDOW
		Text = 8, // COLOR_WINDOWTEXT
		Highlight = 13, // COLOR_HIGHLIGHT
		HighlightText = 14, // COLOR_HIGHLIGHTTEXT
		ButtonFace = 15, // COLOR_3DFACE
		GrayText = 17, // COLOR_GRAYTEXT
		BtnText = 18, // COLOR_BTNTEXT
		Hyperlink = 26, // COLOR_HOTLIGHT
	};

	extern "C" namespace colored_components
	{
		inline constexpr ColoredComponent Text = ColoredComponent::Text;
		inline constexpr ColoredComponent Background = ColoredComponent::Window;
		inline constexpr ColoredComponent Window = ColoredComponent::Window;
		inline constexpr ColoredComponent ButtonText = ColoredComponent::BtnText;
		inline constexpr ColoredComponent ButtonFace = ColoredComponent::ButtonFace;

		inline constexpr ColoredComponent HighlightText = ColoredComponent::HighlightText;
		inline constexpr ColoredComponent HighlightBackground = ColoredComponent::Highlight;
		inline constexpr ColoredComponent GrayText = ColoredComponent::GrayText;
		inline constexpr ColoredComponent Hyperlink = ColoredComponent::Hyperlink;
	}

	const native::RawColorBrush& GetComponentColouring(ColoredComponent) noexcept;

	template<ColoredComponent Target>
	[[nodiscard]]
	const native::RawColorBrush& GetComponentColouring() noexcept
	{
		static const native::RawColorBrush& stock = GetComponentColouring(Target);
		return stock;
	}

	[[nodiscard]]
	const native::RawColorBrush& MakeDefaultComponentColouring() noexcept;
}
