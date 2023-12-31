export module Glib.Windows.ManagedClient;
import <utility>;
import <functional>;
import <memory>;
import <vector>;
import <stack>;
import <unordered_map>;
import Utility.Constraints;
import Utility.Array;
import Utility.Atomic;
import Utility.Monad;
import Utility.Concurrency.Thread;
import Glib.Rect;
import Glib.Windows.Definitions;
import Glib.Windows.IO;
export import Glib.Windows.Event;
export import Glib.Windows.Coroutine;
import Glib.Windows.Client;

namespace gl::win32
{
	// TODO: std::function -> std::move_only_function
	export class ManagedWindow;

	export inline namespace managed_window
	{
		enum class [[nodiscard]] AwakeResult
		{
			Success = 0,
			FailedOnCreatingWorkers = 1,
			FailedOnCreatingCoroutines = 2,
			FailedOnPrepareEvent = 3,
		};

		using KeyDownEventHandler = std::function<void(ManagedWindow&, io::KeyCode, bool is_first)>;
		using KeyUpEventHandler = std::function<void(ManagedWindow&, io::KeyCode)>;
		using SysKeyDownEventHandler = std::function<void(ManagedWindow&, io::KeyCode, bool is_first)>;
		using SysKeyUpEventHandler = std::function<void(ManagedWindow&, io::KeyCode)>;
		using CharDownEventHandler = std::function<void(ManagedWindow&, char32_t, long long)>;
		using CharUpEventHandler = std::function<void(ManagedWindow&, char32_t, long long)>;
		using RenderEventHandler = std::function<void(ManagedWindow&, IContext&)>;
	}

	export class [[nodiscard]] ManagedWindow
		: public std::enable_shared_from_this<ManagedWindow>
	{
	protected:
		using type = ManagedWindow;

		using base_shared_t = std::enable_shared_from_this<type>;
		using under_shared_t = std::shared_ptr<type>;
		using under_weak_t = std::weak_ptr<type>;

		using unit_t = std::unique_ptr<util::jthread>;
		using pool_t = std::vector<unit_t>;
		using coro_t = gl::win32::Coroutine;
		using coro_storage = std::stack<coro_t>;

	public:
		using event_id_t = EventID;
		using event_handler_t = std::function<void(ManagedWindow&, unsigned long long, long long)>;

		using event_t = Event;
		using event_alert_t = std::atomic<event_t>;

		using event_storage_t = std::unordered_map<event_id_t, event_handler_t>;
		using event_iterator = event_storage_t::iterator;
		using event_const_iterator = event_storage_t::const_iterator;

		explicit ManagedWindow(Window&& window, int number_of_workers);

		managed_window::AwakeResult Awake() noexcept;
		void Start() noexcept;
		[[noreturn]]
		void Destroy() noexcept;

		[[nodiscard]] DeviceContext AcquireContext() const noexcept;
		[[nodiscard]] GraphicDeviceContext AcquireRenderContext() const noexcept;

		void SetPowerSave(const bool& flag) noexcept;
		void SetCaptureMouse(const bool& flag = true) noexcept;

		void AddEventHandler(event_id_t id, const event_handler_t& procedure) noexcept;
		void AddEventHandler(event_id_t id, event_handler_t&& procedure) noexcept;
		void RemoveEventHandler(event_id_t id) noexcept;
		void SetRenderer(const RenderEventHandler& handler) noexcept;
		void SetRenderer(RenderEventHandler&& handler) noexcept;

		KeyDownEventHandler SetKeyDownHandler(KeyDownEventHandler handler) noexcept;
		KeyUpEventHandler SetKeyUpHandler(KeyUpEventHandler handler) noexcept;
		SysKeyDownEventHandler SetSysKeyDownHandler(SysKeyDownEventHandler handler) noexcept;
		SysKeyUpEventHandler SetSysKeyUpHandler(SysKeyUpEventHandler handler) noexcept;
		CharDownEventHandler SetCharDownHandler(CharDownEventHandler handler) noexcept;
		CharUpEventHandler SetCharUpHandler(CharUpEventHandler handler) noexcept;
		void StartCoroutine(coro_t&& coroutine) noexcept;

		bool ClearWindow(const Rect& rect) noexcept;
		bool ClearWindow() noexcept;

		[[nodiscard]] std::exception_ptr GetException() const noexcept;

		static long long MainWorker(HWND, unsigned int, unsigned long long, long long) noexcept;
		static void Worker(util::CancellationToken stop_token, ManagedWindow& self, event_alert_t& await_flag) noexcept;

		ManagedWindow(const ManagedWindow&) = delete;
		ManagedWindow(ManagedWindow&&) = delete;
		ManagedWindow& operator=(const ManagedWindow&) = delete;
		ManagedWindow& operator=(ManagedWindow&&) = delete;

	private:
		bool AlertEvent(const event_id_t& event_id, const unsigned long long& lhs, const long long& rhs) noexcept;
		[[nodiscard]]
		util::Monad<event_handler_t> FindEventHandler(const event_id_t& event_id) noexcept;

		bool TryCaptureMouse() noexcept;
		void ResetMouseCapture() noexcept;
		void ClearMouseCapturing() noexcept;
		[[nodiscard]]
		bool IsMouseCaptured() const noexcept;
		void ResumeTopCoroutine() noexcept;

		static void KeyboardHandler(ManagedWindow&, unsigned long long, long long) noexcept;
		static void CharKeyHandler(ManagedWindow&, unsigned long long, long long) noexcept;
		static void DefaultSysKeyEvent(ManagedWindow& self, io::KeyCode code, bool is_first) noexcept;

		Window underlying;
		Rect myDimensions{};

		// flat map
		event_storage_t myEventHandlers{};
		static inline constexpr Event DefaultEvent = {};

		managed_window::KeyDownEventHandler onKeyDown = nullptr;
		managed_window::KeyUpEventHandler onKeyUp = nullptr;
		managed_window::SysKeyDownEventHandler onSysDown = DefaultSysKeyEvent;
		managed_window::SysKeyUpEventHandler onSysUp = nullptr;
		managed_window::CharDownEventHandler onChrDown = nullptr;
		managed_window::CharUpEventHandler onChrUp = nullptr;
		managed_window::RenderEventHandler onRender = nullptr;

		pool_t myWorkers{};
		size_t workerCount = 0;

		util::atomic<int> awaitCount = 0;
		event_alert_t awaitFlag{ DefaultEvent };

		util::atomic_bool isRunning = false;
		util::CancellationSource cancellationSource{};
		util::atomic_int terminateLatch;

		util::atomic_bool isFocused = false;
		util::atomic_bool isMouseHover = false;
		util::atomic_bool isCapturing = false;
		util::atomic_bool isRenderingNow = false;
		util::atomic_bool noPowerSaves = false;

		std::unique_ptr<coro_storage> myCoroutines{};

		std::exception_ptr lastException{};
	};
}
