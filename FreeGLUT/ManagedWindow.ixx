module;

export module Glib.Window.ManagedWindow;
import <cstdio>;
import <iostream>;
import <utility>;
import <memory>;
import <atomic>;
import <vector>;
import <stack>;
import <latch>;
import <unordered_map>;
import Utility.Constraints;
import Utility.Singleton;
import Utility.Reflection.Unique;
import Utility.String;
import Utility.FixedString;
import Utility.Array;
import Utility.Atomic;
import Utility.Monad;
import Utility.Option;
import Utility.Concurrency.Thread;
import Glib.Rect;
import Glib.Window;
import Glib.Device.Event;
import Glib.Device.Utility;
export import Glib.Window.Coroutine;
export import Glib.Device.IO;

export namespace gl::window
{
	template<util::fixed_wstring ID>
	class [[nodiscard]] ManagedWindow
		: public std::enable_shared_from_this<ManagedWindow<ID>>
		, public reflex::Unique<ManagedWindow<ID>, ID>
	{
	protected:
		using type = ManagedWindow<ID>;
		using name_type = decltype(ID);

		using base_shared_t = std::enable_shared_from_this<type>;
		using base_singleton_t = reflex::Unique<ManagedWindow<ID>, ID>;
		using under_shared_t = std::shared_ptr<type>;
		using under_weak_t = std::weak_ptr<type>;

		using unit_t = std::unique_ptr<util::jthread>;
		using pool_t = std::vector<unit_t>;
		using coro_t = Coroutine<ID>;

	public:
		static constexpr device::EventID DefaultEventID = device::EventID::None;
		static constexpr device::Event DefaultEvent = {};

		using event_id_t = device::EventID;
		using event_handler_t = void(*)(ManagedWindow&, unsigned long long, long long);

		using event_t = device::Event;
		using event_alert_t = std::atomic<event_t>;

		using event_storage_t = std::unordered_map<event_id_t, event_handler_t>;
		using event_iterator = event_storage_t::iterator;
		using event_const_iterator = event_storage_t::const_iterator;

		using KeyDownEventHandler = void(*)(ManagedWindow&, device::io::KeyCode, bool is_first);
		using KeyUpEventHandler = void(*)(ManagedWindow&, device::io::KeyCode);
		using SysKeyDownEventHandler = void(*)(ManagedWindow&, device::io::KeyCode, bool is_first);
		using SysKeyUpEventHandler = void(*)(ManagedWindow&, device::io::KeyCode);
		using CharDownEventHandler = void(*)(ManagedWindow&, char32_t, long long);
		using CharUpEventHandler = void(*)(ManagedWindow&, char32_t, long long);

		explicit ManagedWindow(Window&& window, int number_of_workers)
			: underlying(std::move(window))
			, workerCount(number_of_workers), terminateLatch(number_of_workers)
			, windowProcedureHandle(std::move(window.myProcecure))
			, base_shared_t()
		{
			if (base_singleton_t::GetInstance() != nullptr)
			{
				throw std::runtime_error("Only one instance of ManagedWindow is allowed.");
			}
			base_singleton_t::SetInstance(this);

			myDimensions = underlying.GetDimensions();
			myEventHandlers.reserve(20);
			myWorkers.reserve(number_of_workers);
		}

		void Awake() noexcept
		{
			size_t index = 0;
			for (unit_t& worker : myWorkers)
			{
				worker = std::make_unique<util::jthread>(ManagedWindow::Worker, cancellationSource.get_token(), util::ref(*this), util::ref(awaitFlag));

				++index;
			}

			underlying.Awake();
			underlying.Start();
		}

		/// <summary>
		/// The main loop
		/// </summary>
		void Start() noexcept
		{
			isRunning = true;

			while (true)
			{
				if (cancellationSource.stop_requested())
				{
					break;
				}

				underlying.UpdateOnce();
			}
		}

		void AddEventHandler(event_id_t id, const event_handler_t& procedure) noexcept
		{
			myEventHandlers.insert(std::make_pair(id, procedure));
		}

		void SetCaptureMouse(const bool& flag = true) noexcept
		{
			isCapturing = flag;
		}

		void StartCoroutine(coro_t&& coroutine) noexcept
		{
			if (0 < myCoroutines.size())
			{
				myCoroutines.top()->Resume();
			}

			myCoroutines.push(std::make_unique<coro_t>(std::move(coroutine)));
		}

		[[noreturn]]
		void Destroy() noexcept
		{
			if (isRunning)
			{
				cancellationSource.request_stop();
				awaitFlag.notify_all();

				for (unit_t& worker : myWorkers)
				{
					worker->detach();
				}

				isRunning = false;
			}
		}

		static long long MainWorker(device::HWND, unsigned int, unsigned long long, long long) noexcept;

		static void Worker(util::CancellationToken stop_token, ManagedWindow& self, event_alert_t& await_flag) noexcept;

		ManagedWindow(const ManagedWindow&) = delete;
		ManagedWindow(ManagedWindow&&) noexcept = delete;
		ManagedWindow& operator=(const ManagedWindow&) = delete;
		ManagedWindow& operator=(ManagedWindow&&) noexcept = delete;

	private:
		bool AlertEvent(const event_id_t& event_id, const unsigned long long& lhs, const long long& rhs) noexcept
		{
			if (myEventHandlers.contains(event_id))
			{
				awaitFlag.store(event_t(event_id, lhs, rhs, 0));
				awaitFlag.notify_one();
				return true;
			}
			else
			{
				return false;
			}
		}

		[[nodiscard]]
		util::Monad<event_handler_t> FindEventHandler(const event_id_t& event_id)
			noexcept
		{
			const event_iterator it = myEventHandlers.find(event_id);
			if (it == myEventHandlers.cend())
			{
				return util::nullopt;
			}
			else
			{
				return it->second;
			}
		}

		/// <summary>
		/// Only on the main thread
		/// </summary>
		bool TryCaptureMouse() noexcept
		{
			if (isCapturing)
			{
				detail::CaptureMouse(underlying.GetHandle());
				return true;
			}
			else
			{
				return false;
			}
		}

		/// <summary>
		/// Only on the main thread
		/// </summary>
		void ResetMouseCapture() noexcept
		{
			detail::ResetMouseCapture(underlying.GetHandle());
		}

		/// <summary>
		/// Only on the main thread
		/// </summary>
		void ClearMouseCapturing() noexcept
		{
			ResetMouseCapture();
			isCapturing = false;
		}

		/// <summary>
		/// Only on the main thread
		/// </summary>
		[[nodiscard]]
		bool IsMouseCaptured() const noexcept
		{
			return detail::IsMouseCaptured(underlying.GetHandle());
		}

		static void DefaultSysKeyEvent(ManagedWindow& self, device::io::KeyCode code, bool is_first) noexcept
		{
			if (is_first && code == device::io::KeyCode::F4)
			{
				self.underlying.SendCommand(event_id_t::Close);
			}
		}

		Window underlying;
		Rect myDimensions{};
		WindowProcedure windowProcedureHandle = nullptr;

		// flat map
		event_storage_t myEventHandlers{};

		pool_t myWorkers{};
		size_t workerCount = 0;

		util::atomic_bool isRunning = false;
		util::CancellationSource cancellationSource{};
		std::latch terminateLatch;

		util::atomic<int> awaitCount = 0;
		event_alert_t awaitFlag{ DefaultEvent };

		KeyDownEventHandler keyDownHandler = nullptr;
		KeyUpEventHandler keyUpHandler = nullptr;
		SysKeyDownEventHandler sysDownHandler = DefaultSysKeyEvent;
		SysKeyUpEventHandler sysUpHandler = nullptr;
		CharDownEventHandler chrDownHandler = nullptr;
		CharUpEventHandler chrUpHandler = nullptr;

		util::atomic_bool isFocused = false;
		util::atomic_bool isCapturing = false;
		util::Option<bool> optionFullscreen{ false };
		util::atomic_bool isRenderingNow = false;

		std::vector<std::unique_ptr<coro_t>> myCoroutines{};
	};

#pragma region CreateWindowEx
	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(const WindowProperty& properties
		, const std::wstring_view& title
		, const WindowStyle& style
		, const WindowOption& option
		, const int& x
		, const int& y
		, const int& width
		, const int& height
	)
	{
		return ManagedWindow<NID>{ CreateWindow(properties, title, style, option, x, y, width, height), Workers };
	}

	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(WindowProperty&& properties
		, const std::wstring_view& title
		, const WindowStyle& style
		, const WindowOption& option
		, const int& x
		, const int& y
		, const int& width
		, const int& height
	)
	{
		return ManagedWindow<NID>{ CreateWindow(std::move(properties), title, style, option, x, y, width, height), Workers };
	}

	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(const WindowProperty& properties
		, const std::wstring_view& title
		, const WindowStyle& style
		, const WindowOption& option
		, const Rect& dimension
	)
	{
		return ManagedWindow<NID>{ CreateWindow(properties, title, style, option, dimension), Workers };
	}

	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(WindowProperty&& properties
		, const std::wstring_view& title
		, const WindowStyle& style
		, const WindowOption& option
		, const Rect& dimension
	)
	{
		return ManagedWindow<NID>{ CreateWindow(std::move(properties), title, style, option, dimension), Workers };
	}

	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(const WindowProperty& properties
		, const std::wstring_view& title
		, const WindowStyle& style
		, const int& x
		, const int& y
		, const int& width
		, const int& height
	)
	{
		return ManagedWindow<NID>{ CreateWindow(properties, title, style, x, y, width, height), Workers };
	}

	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(WindowProperty&& properties
		, const std::wstring_view& title
		, const WindowStyle& style
		, const int& x
		, const int& y
		, const int& width
		, const int& height
	)
	{
		return ManagedWindow<NID>{ CreateWindow(std::move(properties), title, style, x, y, width, height), Workers };
	}

	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(const WindowProperty& properties
		, const std::wstring_view& title
		, const WindowStyle& style
		, const Rect& dimension
	)
	{
		return ManagedWindow<NID>{ CreateWindow(properties, title, style, dimension), Workers };
	}

	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(WindowProperty&& properties
		, const std::wstring_view& title
		, const WindowStyle& style
		, const Rect& dimension
	)
	{
		return ManagedWindow<NID>{ CreateWindow(std::move(properties), title, style, options::Default, dimension), Workers };
	}

	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(const WindowProperty& properties
		, const std::wstring_view& title
		, const int& x
		, const int& y
		, const int& width
		, const int& height
	)
	{
		return ManagedWindow<NID>{ CreateWindow(properties, title, x, y, width, height), Workers };
	}

	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(WindowProperty&& properties
		, const std::wstring_view& title
		, const int& x
		, const int& y
		, const int& width
		, const int& height
	)
	{
		return ManagedWindow<NID>{ CreateWindow(std::move(properties), title, x, y, width, height), Workers };
	}

	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(const WindowProperty& properties
		, const std::wstring_view& title
		, const Rect& dimension
	)
	{
		return ManagedWindow<NID>{ CreateWindow(properties, title, dimension), Workers };
	}

	template<util::basic_fixed_string NID, size_t Workers = 4>
	[[nodiscard]]
	ManagedWindow<NID> CreateWindowEx(WindowProperty&& properties
		, const std::wstring_view& title
		, const Rect& dimension
	)
	{
		return ManagedWindow<NID>{ CreateWindow(std::move(properties), title, dimension), Workers };
	}

	template<util::fixed_wstring NID>
	[[nodiscard]]
	WindowProperty CreatePropertyEx() noexcept
	{
		constexpr std::wstring_view class_name_view{ NID };

		return gl::window::CreateProperty(device::GetProcessInstance(), ManagedWindow<NID>::MainWorker, class_name_view.data());
	}

	template<util::fixed_wstring NID, typename IconType, typename CursorType>
	[[nodiscard]]
	WindowProperty CreatePropertyEx(IconType&& icon
		, IconType&& small_icon
		, CursorType&& cursor
		, const ::HBRUSH& background
		, const wchar_t* const& menu_name
	) noexcept
	{
		constexpr std::wstring_view class_name_view{ NID };

		return gl::window::CreateProperty(device::GetProcessInstance(), ManagedWindow<NID>::MainWorker
			, class_name_view.data()
			, std::forward<IconType>(icon), std::forward<IconType>(small_icon)
			, std::forward<CursorType>(cursor)
			, background
			, menu_name);
	}
#pragma endregion
}

#pragma warning(push)
template<util::fixed_wstring ID>
long long
gl::window::ManagedWindow<ID>::MainWorker(device::HWND hwnd // underlying.myHandle
, unsigned int id
, unsigned long long wparam, long long lparam)
noexcept
{
	const event_id_t msg = static_cast<event_id_t>(id);

	ManagedWindow<ID>* self = ManagedWindow<ID>::GetInstance();
	auto& key_dw_handler = self->keyDownHandler;
	auto& key_up_handler = self->keyUpHandler;
	auto& sys_dw_handler = self->sysDownHandler;
	auto& sys_up_handler = self->sysUpHandler;
	auto& chr_dw_handler = self->chrDownHandler;
	auto& chr_up_handler = self->chrUpHandler;

	switch (msg)
	{
		case gl::device::KeyboardEventID::Pressed:
		{
			std::printf("[Key Pressed] %lld\n", wparam);
			if (key_dw_handler)
			{
				key_dw_handler(*self, static_cast<device::io::KeyCode>(wparam), device::io::IsFirstPress(lparam));
			}
		}
		break;

		case gl::device::KeyboardEventID::Released:
		{
			std::printf("[Key Released] %lld\n", wparam);
			if (key_up_handler)
			{
				key_up_handler(*self, static_cast<device::io::KeyCode>(wparam));
			}
		}
		break;

		case gl::device::KeyboardEventID::AltPressed:
		{
			std::printf("[System Key Pressed] %lld\n", wparam);
			if (sys_dw_handler)
			{
				sys_dw_handler(*self, static_cast<device::io::KeyCode>(wparam), device::io::IsFirstPress(lparam));
			}
		}
		break;

		case gl::device::KeyboardEventID::AltReleased:
		{
			std::printf("[System Key Released] %lld\n", wparam);
			if (sys_up_handler)
			{
				sys_up_handler(*self, static_cast<device::io::KeyCode>(wparam));
			}
		}
		break;

		case gl::device::KeyboardEventID::CharPressed:
		case gl::device::KeyboardEventID::AltCharPressed:
		{
			std::printf("[Chr Pressed] %lld\n", wparam);
			if (chr_dw_handler)
			{
				chr_dw_handler(*self, static_cast<char32_t>(wparam), lparam);
			}
		}
		break;

		case gl::device::KeyboardEventID::CharReleased:
		case gl::device::KeyboardEventID::AltCharReleased:
		{
			std::printf("[Chr Released] %lld\n", wparam);
			if (chr_up_handler)
			{
				chr_up_handler(*self, static_cast<char32_t>(wparam), lparam);
			}
		}
		break;

		case event_id_t::Activate:
		{
			const unsigned short trigger = device::LOWORD(wparam);

			if (trigger == device::DeviceActivation::Inactivated)
			{
				std::printf("[Activate] Unfocused\n");
				self->isFocused = false;
				self->ResetMouseCapture();
			}
			else
			{
				std::printf("[Activate] Focused\n");
				self->isFocused = true;
				self->TryCaptureMouse();
			}
		}
		break;

		case event_id_t::SetKeyboardFocus:
		{
			std::printf("[SetKeyboardFocus]\n");
		}
		break;

		case event_id_t::KillKeyboardFocus:
		{
			std::printf("[KillKeyboardFocus]\n");
		}
		break;

		// Started by close button or system menu [or Alt+F4]
		case event_id_t::Close:
		{
			detail::DestroyNativeWindow(hwnd);
			self->isFocused = false;
			self->ClearMouseCapturing();
		}
		break;

		// Started by DestroyWindow
		case event_id_t::Destroy:
		{
			device::PostQuitMessage(0);

			if (self)
			{
				self->Destroy();
			}
		}
		break;

		// Started by WM_DESTROY
		case event_id_t::Quit:
		{
			//KillTimer(hwnd, RENDER_TIMER_ID);
		}
		break;

		// Clean memory up
		// Started by WM_DESTROY
		case event_id_t::CleanupMemory:
		{}
		break;

		//[[fallthrough]]
		default:
		{
			if (self)
			{
				if (self->AlertEvent(msg, wparam, lparam))
				{
					break;
				}
			}

			return detail::DefaultWindowsProcedure(hwnd, id, wparam, lparam);
		}
		break;
	}

	return 0;
}

#pragma warning(disable: 4189)
template<util::fixed_wstring ID>
void gl::window::ManagedWindow<ID>::Worker(util::CancellationToken stop_token, ManagedWindow& self, event_alert_t& await_flag) noexcept
{
	await_flag.wait(DefaultEvent, util::memory_order_relaxed);

	while (true)
	{
		if (stop_token.stop_requested())
		{
			return;
		}

		auto event = await_flag.load();
		self.FindEventHandler(event.id).if_then([&](const event_handler_t& handler) noexcept {
			handler(self, event.wParam, event.lParam);
		});

		await_flag.store(DefaultEvent, util::memory_order_acquire);
		await_flag.wait(DefaultEvent, util::memory_order_release);
	}

	self.terminateLatch.arrive_and_wait();
}
#pragma warning(pop)
