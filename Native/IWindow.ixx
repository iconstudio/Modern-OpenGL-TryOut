export module Glib.Device.IWindow;
import <type_traits>;
import <string>;
import <string_view>;
import Glib.Rect;
import Glib.Definitions;
import Glib.Device.ProcessInstance;
import Glib.Device.IHandle;
import Glib.Device.Event;
import Glib.Device.IO;

export namespace gl::device
{
	using native::HWND;

	class [[nodiscard]] IWindow : public IHandle<native::HWND>
	{
	public:
		using base = IHandle<native::HWND>;
		using handle_type = base::handle_type;

		constexpr IWindow() noexcept = default;

		constexpr IWindow(nullptr_t) noexcept
			: base(nullptr)
		{}

		constexpr IWindow(IWindow&& other) noexcept
			: base(std::move(other))
		{}

		explicit constexpr IWindow(const handle_type& handle) noexcept
			: base(handle)
		{}

		explicit constexpr IWindow(handle_type&& handle) noexcept
			: base(std::move(handle))
		{}

		constexpr IWindow& operator=(IWindow&& other) noexcept
		{
			base::operator=(std::exchange(other, nullptr));
			return *this;
		}

		constexpr IWindow& operator=(handle_type&& handle) noexcept
		{
			base::operator=(std::move(handle));
			return *this;
		}

		bool StartUpdate() noexcept;

		long long DefaultWndProc(const unsigned int& msg, const unsigned long long& lhs, const long long& rhs) const noexcept;

		bool Destroy() noexcept;

		bool SendCommand(const unsigned int& id, const unsigned long long& lhs, const long long& rhs) const noexcept;
		bool SendCommand(const gl::device::EventID& id, const unsigned long long& lhs, const long long& rhs) const noexcept;
		bool SendCommand(const gl::device::Event& cmd) const noexcept;
		bool SendCommand(gl::device::Event&& cmd) const noexcept;
		bool SendCommand(const gl::device::EventID& id, const io::KeyCode& keycode, const io::KeyboardFlag& flags = io::KeyboardFlag::None) const noexcept;

		bool UICommand(const int& cmd) noexcept;
		bool Show() noexcept;
		bool Hide() noexcept;
		bool Close() noexcept;

		bool Maximize() noexcept;
		bool Minimize() noexcept;
		bool Restore() noexcept;

		bool Redraw(const bool& flag) noexcept;

		bool EnableInput() noexcept;
		bool DisableInput() noexcept;

		[[nodiscard]] DeviceContext AcquireContext() const noexcept;
		[[nodiscard]] RenderingContext AcquireRenderContext() const noexcept;
		bool ReleaseContext(DeviceContext& context) const noexcept;
		[[nodiscard]] IContext AcquireNativeContext() const noexcept;
		bool ReleaseNativeContext(IContext& context) const noexcept;

		[[nodiscard]] unsigned long GetStyle() const noexcept;
		[[nodiscard]] unsigned long GetExStyle() const noexcept;
		[[nodiscard]] int GetID() const noexcept;
		[[nodiscard]] std::wstring GetTitle() const noexcept;

		long long SetInternalValue(int index, const long long& value) const noexcept;
		long long SetInternalValue(int index, long long&& value) const noexcept;
		long long SetInternalUserData(const long long& value) const noexcept;
		long long SetInternalUserData(long long&& value) const noexcept;
		[[nodiscard]] long long GetInternalValue(int index) const noexcept;
		[[nodiscard]] long long GetInternalUserData() const noexcept;

		[[nodiscard]] Rect GetDimensions() const noexcept;
		bool TryGetDimensions(Rect& output) const noexcept;

		[[nodiscard]] bool IsMinimized() const noexcept;
		[[nodiscard]] bool IsMaximized() const noexcept;
		[[nodiscard]] bool IsRestored() const noexcept;
		[[nodiscard]] bool IsInputEnabled() const noexcept;

		IWindow(const IWindow&) = delete;
		IWindow& operator=(const IWindow&) = delete;
	};

	[[nodiscard]]
	IWindow MakeNativeWindow(const ProcessInstance& hinst
		, const std::wstring_view& class_name
		, const std::wstring_view& title
		, const unsigned long& styles
		, const unsigned long& options
		, const int& x
		, const int& y
		, const int& width
		, const int& height
		, const IWindow& parent = nullptr
		, const native::NativeMenu& menu = nullptr
		, void* uparams = nullptr)
		noexcept;

	void PostQuitMessage(const int& exit_code) noexcept;
}