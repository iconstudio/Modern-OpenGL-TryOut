export module Glib.Windows.Context;
import <type_traits>;
import Glib.Windows.Definitions;
export import Glib.Windows.IContext;

export namespace gl::win32
{
	class [[nodiscard]] DeviceContext : public IContext
	{
	public:
		using base = IContext;

		DeviceContext(const native::HWND& window) noexcept;
		~DeviceContext() noexcept;

		DeviceContext(const DeviceContext&) = delete;
		constexpr DeviceContext(DeviceContext&&) noexcept = default;
		DeviceContext& operator=(const DeviceContext&) = delete;
		constexpr DeviceContext& operator=(DeviceContext&&) noexcept = default;

	private:
		using base::Destroy;

		native::HWND myWindowHandle;
	};

	class [[nodiscard]] GlobalDeviceContext : public IContext
	{
	public:
		using base = IContext;

		GlobalDeviceContext() noexcept;
		~GlobalDeviceContext() noexcept;

		GlobalDeviceContext(const GlobalDeviceContext&) = delete;
		constexpr GlobalDeviceContext(GlobalDeviceContext&&) noexcept = default;
		GlobalDeviceContext& operator=(const GlobalDeviceContext&) = delete;
		constexpr GlobalDeviceContext& operator=(GlobalDeviceContext&&) noexcept = default;

	private:
		using base::Destroy;
		using base::Release;
	};
}
