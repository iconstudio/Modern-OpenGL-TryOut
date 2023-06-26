module;
#include "Internal.hpp"

export module Glib.Device.Handle;
import <type_traits>;
import Glib.Device.Command;

export namespace gl::device
{
	using RawDeviceHandle = ::HWND__*;
	using ::RECT;
	using ::LPRECT;

	using ::GetLastError;

	class [[nodiscard]] DeviceHandle
	{
	public:
		constexpr DeviceHandle() noexcept = default;

		constexpr DeviceHandle(nullptr_t) noexcept
			: myHandle(nullptr)
		{}

		constexpr DeviceHandle(RawDeviceHandle&& handle) noexcept
			: myHandle(static_cast<RawDeviceHandle&&>(handle))
		{}

		constexpr DeviceHandle(volatile RawDeviceHandle&& handle) noexcept
			: myHandle(static_cast<volatile RawDeviceHandle&&>(handle))
		{}

		~DeviceHandle() noexcept
		{
			if (myHandle)
			{
				::DestroyWindow(myHandle);
				myHandle = nullptr;
			}
		}

		inline bool StartUpdate() noexcept
		{
			return 0 != ::UpdateWindow(myHandle);
		}

		inline bool SendCommand(const unsigned int& msg, const WPARAM& lhs, const LPARAM& rhs) const
			noexcept
		{
			return DeviceCommandAPI::Push(myHandle, msg, lhs, rhs);
		}

		inline bool UICommand(const int& cmd) noexcept
		{
			return SendCommand(WM_SHOWWINDOW, static_cast<WPARAM>(cmd), 0);
		}

		inline bool Close() noexcept
		{
			return SendCommand(WM_CLOSE, 0, 0);
		}

		inline bool Show() noexcept
		{
			return 0 != ::ShowWindow(myHandle, SW_SHOW);
		}

		inline bool Hide() noexcept
		{
			return 0 != ::ShowWindow(myHandle, SW_HIDE);
		}

		inline bool Maximize() noexcept
		{
			return UICommand(SW_MAXIMIZE);
		}

		inline bool Minimize() noexcept
		{
			return UICommand(SW_MINIMIZE);
		}

		inline bool Restore() noexcept
		{
			return UICommand(SW_RESTORE);
		}

		inline bool MakeFocus() noexcept
		{
			return SendCommand(WM_SETFOCUS, TRUE, 0);
		}

		inline bool MakeForeground() noexcept
		{
			return SendCommand(WM_ACTIVATE, WA_ACTIVE, 0);
		}

		inline bool Redraw(const bool& flag) noexcept
		{
			return SendCommand(WM_SETREDRAW, static_cast<WPARAM>(flag), 0);
		}

		inline bool EnableInput() noexcept
		{
			return 0 != ::EnableWindow(myHandle, TRUE);
		}

		inline bool DisableInput() noexcept
		{
			return 0 != ::EnableWindow(myHandle, FALSE);
		}

		[[nodiscard]]
		inline bool IsMinimized() const noexcept
		{
			return 0 != IsIconic(myHandle);
		}

		[[nodiscard]]
		inline bool IsMaximized() const noexcept
		{
			return 0 != IsZoomed(myHandle);
		}

		[[nodiscard]]
		inline bool IsRestored() const noexcept
		{
			return 0L == (GetStyle() & (WS_MINIMIZE | WS_MAXIMIZE));
		}

		[[nodiscard]]
		inline bool IsInputEnabled() const noexcept
		{
			return 0 != ::IsWindowEnabled(myHandle);
		}

		[[nodiscard]]
		inline unsigned long GetStyle() const noexcept
		{
			return static_cast<DWORD>(GetWindowLongPtr(myHandle, GWL_STYLE));
		}

		[[nodiscard]]
		inline unsigned long GetExStyle() const noexcept
		{
			return static_cast<DWORD>(GetWindowLongPtr(myHandle, GWL_EXSTYLE));
		}

		[[nodiscard]]
		inline int GetID() const noexcept
		{
			return static_cast<int>(GetWindowLongPtr(myHandle, GWLP_ID));
		}

		[[nodiscard]]
		inline RECT GetDimension() const noexcept
		{
			RECT rect;
			::GetWindowRect(myHandle, &rect);
			return rect;
		}

		[[nodiscard]]
		inline bool TryGetDimension(RECT& output) const noexcept
		{
			return 0 != ::GetWindowRect(myHandle, &output);
		}
	protected:
		[[nodiscard]]
		static constexpr RECT MakeNativeRect() noexcept
		{
			return {};
		}

	public:
		[[nodiscard]]
		inline RawDeviceHandle GetOwner() const noexcept
		{
			return ::GetWindow(myHandle, GW_OWNER);
		}

		[[nodiscard]]
		inline RawDeviceHandle GetFirstChild() const noexcept
		{
			return ::GetTopWindow(myHandle);
		}

		[[nodiscard]]
		inline RawDeviceHandle GetFirstSibling() const noexcept
		{
			return ::GetWindow(myHandle, GW_HWNDFIRST);
		}

		[[nodiscard]]
		inline RawDeviceHandle GetLastChild() const noexcept
		{
			return ::GetWindow(myHandle, GW_CHILD);
		}

		[[nodiscard]]
		inline RawDeviceHandle GetLastSibling() const noexcept
		{
			return ::GetWindow(myHandle, GW_HWNDLAST);
		}

		[[nodiscard]]
		inline RawDeviceHandle GetNextSibling() const noexcept
		{
			return ::GetWindow(myHandle, GW_HWNDNEXT);
		}

		[[nodiscard]]
		inline RawDeviceHandle GetPrevSibling() const noexcept
		{
			return ::GetWindow(myHandle, GW_HWNDPREV);
		}

		[[nodiscard]]
		inline RawDeviceHandle GetParent() const noexcept
		{
			return ::GetParent(myHandle);
		}

		[[nodiscard]]
		inline RawDeviceHandle GetRoot() const noexcept
		{
			return ::GetAncestor(myHandle, GA_ROOT);
		}

		[[nodiscard]]
		inline RawDeviceHandle GetRootOwner() const noexcept
		{
			return ::GetAncestor(myHandle, GA_ROOTOWNER);
		}

		[[nodiscard]]
		inline const RawDeviceHandle& GetHandle() const& noexcept
		{
			return myHandle;
		}

		[[nodiscard]]
		inline RawDeviceHandle&& GetHandle() && noexcept
		{
			return static_cast<RawDeviceHandle&&>(myHandle);
		}

		constexpr operator RawDeviceHandle() const noexcept
		{
			return myHandle;
		}

		constexpr DeviceHandle& operator=(RawDeviceHandle&& handle) noexcept
		{
			myHandle = static_cast<RawDeviceHandle&&>(handle);
			return *this;
		}

		[[nodiscard]]
		constexpr bool operator==(const DeviceHandle& rhs) const noexcept
		{
			return myHandle == rhs.myHandle;
		}

		DeviceHandle(const DeviceHandle&) = delete;
		constexpr DeviceHandle(DeviceHandle&&) noexcept = default;
		DeviceHandle& operator=(const DeviceHandle&) = delete;
		constexpr DeviceHandle& operator=(DeviceHandle&&) noexcept = default;

	private:
		RawDeviceHandle myHandle = nullptr;
	};

#define IsLButtonDown()  (GetKeyState(VK_LBUTTON) < 0)
#define IsRButtonDown()  (GetKeyState(VK_RBUTTON) < 0)
#define IsMButtonDown()  (GetKeyState(VK_MBUTTON) < 0)
}
