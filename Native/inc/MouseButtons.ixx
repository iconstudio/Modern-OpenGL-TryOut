export module Glib.Windows.IO.Mouse:Buttons;

export namespace gl::win32::io
{
	enum class [[nodiscard]] MouseButtons
	{
		Left = 0,
		Middle = 1,
		Right = 2
	};

	enum class [[nodiscard]] MouseState
	{
		Down = 0,
		Up = 1
	};

	enum class [[nodiscard]] MouseEntry
	{
		Left = 0,
		Entered = 1
	};
}
