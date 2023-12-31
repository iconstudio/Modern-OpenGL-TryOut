export module Glib:Pipeline;
import <memory>;
import <vector>;
import <functional>;
import :Object;
import :Shader;
import :Primitive;

export namespace gl
{
	namespace pipeline
	{
		struct [[nodiscard]] noopt_t
		{
			explicit noopt_t() noexcept = default;
		};
	}

	inline constexpr pipeline::noopt_t noopt{};

	class [[nodiscard]] Pipeline final : public gl::Object
	{
	private:
		using base = gl::Object;
		
	public:
		using shader_t = gl::Shader;
		using shader_handle_t = std::unique_ptr<shader_t>;
		using renderer_t = std::move_only_function<void() noexcept>;

		static inline constexpr size_t DefaultReservedShaders = 8;

		Pipeline() noexcept;
		~Pipeline() noexcept;

		constexpr Pipeline(pipeline::noopt_t) noexcept
			: myShaders{}
		{
			myShaders.reserve(DefaultReservedShaders);
		}

		bool Awake() volatile noexcept;
		bool Start() const volatile noexcept;
		void Use() volatile noexcept;
		void Render(Primitive pr, const std::uint32_t& vertices_count) const volatile noexcept;
		void Destroy() noexcept;

		void AddShader(shader_t&& shader);
		void AddShader(shader_handle_t&& shader);

		[[nodiscard]] size_t GetNumberOfShaders() const noexcept;

		Pipeline(const Pipeline&) = delete;
		Pipeline(Pipeline&&) noexcept = default;
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline& operator=(Pipeline&&) noexcept = default;

	private:
		std::vector<shader_handle_t> myShaders;
	};
}
