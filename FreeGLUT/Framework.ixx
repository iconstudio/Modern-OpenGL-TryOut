export module Glib.Framework;
import <memory>;
import <string_view>;
import Glib.Rect;
import Glib.GraphicContext;
import Glib.Window.ManagedWindow;

export namespace gl
{
	namespace framework
	{
		using handle_t = gl::window::ManagedWindow;

		struct Descriptor
		{
			std::wstring_view title;
			int wx, wy, ww, wh;
			bool isPowersave = false;
		};

		enum class InitError
		{
			Success,
			FailedOnCreatingWindow,
			FailedOnSettingPixelFormat,
		};

		[[nodiscard]]
		consteval Descriptor MakeDefaultDescriptor() noexcept
		{
			return Descriptor
			{
				.title = L"FreeGLUT",
				.wx = static_cast<int>(0x80000000), // CW_USEDEFAULT
				.wy = 0,
				.ww = 800,
				.wh = 600
			};
		}

		inline constexpr Descriptor DefaultDescriptor = MakeDefaultDescriptor();
	}

	class [[nodiscard]] Framework : public std::enable_shared_from_this<Framework>
	{
	public:
		using base = std::enable_shared_from_this<Framework>;

		constexpr Framework() noexcept = default;
		~Framework() noexcept = default;

		framework::InitError Initialize(const framework::Descriptor& setup);
		framework::InitError Initialize(framework::Descriptor&& setup);
		framework::InitError Initialize();
		void Run() noexcept;

	private:
		std::unique_ptr<framework::handle_t> myInstance;
		gl::Rect window_rect{};
	};
}
