export module Glib.Device.IHandle;
import Utility.Constraints;

export namespace gl::device
{
	template<typename H>
	struct [[nodiscard]] IHandle;

	template<typename H>
	struct [[nodiscard]] IHandle<H*>
	{
		using handle_type = H*;
		using pointer_type = H*;
		using value_type = H;

		constexpr IHandle() noexcept = default;
		constexpr ~IHandle() = default;

		explicit constexpr IHandle(handle_type const& handle) noexcept
			: myHandle(handle)
		{}

		explicit constexpr IHandle(handle_type&& handle) noexcept
			: myHandle(std::move(handle))
		{}

		constexpr IHandle& operator=(handle_type&& handle) noexcept
		{
			myHandle = std::move(handle);
			return *this;
		}

		template<util::invocables<pointer_type&> Fn>
		constexpr auto Delegate(Fn&& fn) &
			noexcept(util::nothrow_invocables<Fn, handle_type&>)
		{
			if constexpr (util::is_same_v<util::invoke_result_t<Fn, handle_type&>, void>)
			{
				std::invoke(std::forward<Fn>(fn), myHandle);
			}
			else
			{
				return std::invoke(std::forward<Fn>(fn), myHandle);
			}
		}

		template<util::invocables<const pointer_type&> Fn>
		constexpr auto Delegate(Fn&& fn) const&
			noexcept(util::nothrow_invocables<Fn, const handle_type&>)
		{
			if constexpr (util::is_same_v<util::invoke_result_t<Fn, const handle_type&>, void>)
			{
				std::invoke(std::forward<Fn>(fn), myHandle);
			}
			else
			{
				return std::invoke(std::forward<Fn>(fn), myHandle);
			}
		}

		template<util::invocables<pointer_type&&> Fn>
		constexpr auto Delegate(Fn&& fn) &&
			noexcept(util::nothrow_invocables<Fn, handle_type&&>)
		{
			if constexpr (util::is_same_v<util::invoke_result_t<Fn, handle_type&&>, void>)
			{
				std::invoke(std::forward<Fn>(fn), std::move(myHandle));
			}
			else
			{
				return std::invoke(std::forward<Fn>(fn), std::move(myHandle));
			}
		}

		template<util::invocables<const pointer_type&&> Fn>
		constexpr auto Delegate(Fn&& fn) const&&
			noexcept(util::nothrow_invocables<Fn, const handle_type&>)
		{
			if constexpr (util::is_same_v<util::invoke_result_t<Fn, const handle_type&&>, void>)
			{
				std::invoke(std::forward<Fn>(fn), std::move(myHandle));
			}
			else
			{
				return std::invoke(std::forward<Fn>(fn), std::move(myHandle));
			}
		}

		[[nodiscard]]
		constexpr handle_type& GetHandle() & noexcept
		{
			return myHandle;
		}

		[[nodiscard]]
		constexpr handle_type const& GetHandle() const& noexcept
		{
			return myHandle;
		}

		[[nodiscard]]
		constexpr handle_type&& GetHandle() && noexcept
		{
			return std::move(myHandle);
		}

		[[nodiscard]]
		constexpr handle_type const&& GetHandle() const&& noexcept
		{
			return std::move(myHandle);
		}

		explicit constexpr operator handle_type& () & noexcept
		{
			return myHandle;
		}

		explicit constexpr operator handle_type const& () const& noexcept
		{
			return myHandle;
		}

		explicit constexpr operator handle_type && () && noexcept
		{
			return std::move(myHandle);
		}

		explicit constexpr operator handle_type const&& () const&& noexcept
		{
			return std::move(myHandle);
		}

		[[nodiscard]]
		constexpr bool operator==(const IHandle& other) const noexcept
		{
			return myHandle == other.myHandle;
		}

		[[nodiscard]]
		constexpr bool operator==(handle_type const& handle) const noexcept
		{
			return myHandle == handle;
		}

		IHandle(const IHandle&) = delete;
		constexpr IHandle(IHandle&&) noexcept = default;
		IHandle& operator=(const IHandle&) = delete;
		constexpr IHandle& operator=(IHandle&&) noexcept = default;

		handle_type myHandle;
	};

	template<util::movable H>
	struct [[nodiscard]] IHandle<H>
	{
		using handle_type = H;

		constexpr IHandle()
			noexcept(util::nothrow_default_constructibles<handle_type>)
			requires util::default_initializable<handle_type> = default;
		constexpr ~IHandle()
			noexcept(util::nothrow_destructibles<handle_type>) = default;

		explicit(util::is_explicit_constructible_v<handle_type>)
			constexpr IHandle(const handle_type& handle)
			noexcept(util::nothrow_copy_constructibles<handle_type>) requires util::copyable<handle_type>
			: myHandle(handle)
		{}

		explicit(util::is_explicit_constructible_v<handle_type>)
			constexpr IHandle(handle_type&& handle)
			noexcept(util::nothrow_move_constructibles<handle_type>)
			: myHandle(std::move(handle))
		{}

		constexpr IHandle& operator=(handle_type&& handle) noexcept
		{
			myHandle = std::move(handle);
			return *this;
		}

		template<typename Fn>
		constexpr auto Delegate(Fn&& fn) &
			noexcept(util::nothrow_invocables<Fn, handle_type&>)
		{
			if constexpr (util::is_same_v<util::invoke_result_t<Fn, handle_type&>, void>)
			{
				std::invoke(std::forward<Fn>(fn), myHandle);
			}
			else
			{
				return std::invoke(std::forward<Fn>(fn), myHandle);
			}
		}

		template<typename Fn>
		constexpr auto Delegate(Fn&& fn) const&
			noexcept(util::nothrow_invocables<Fn, const handle_type&>)
		{
			if constexpr (util::is_same_v<util::invoke_result_t<Fn, const handle_type&>, void>)
			{
				std::invoke(std::forward<Fn>(fn), myHandle);
			}
			else
			{
				return std::invoke(std::forward<Fn>(fn), myHandle);
			}
		}

		template<typename Fn>
		constexpr auto Delegate(Fn&& fn) &&
			noexcept(util::nothrow_invocables<Fn, handle_type&&>)
		{
			if constexpr (util::is_same_v<util::invoke_result_t<Fn, handle_type&&>, void>)
			{
				std::invoke(std::forward<Fn>(fn), std::move(myHandle));
			}
			else
			{
				return std::invoke(std::forward<Fn>(fn), std::move(myHandle));
			}
		}

		template<typename Fn>
		constexpr auto Delegate(Fn&& fn) const&&
			noexcept(util::nothrow_invocables<Fn, const handle_type&>)
		{
			if constexpr (util::is_same_v<util::invoke_result_t<Fn, const handle_type&&>, void>)
			{
				std::invoke(std::forward<Fn>(fn), std::move(myHandle));
			}
			else
			{
				return std::invoke(std::forward<Fn>(fn), std::move(myHandle));
			}
		}

		[[nodiscard]]
		constexpr handle_type& GetHandle() & noexcept
		{
			return myHandle;
		}

		[[nodiscard]]
		constexpr const handle_type& GetHandle() const& noexcept
		{
			return myHandle;
		}

		[[nodiscard]]
		constexpr handle_type&& GetHandle() && noexcept
		{
			return std::move(myHandle);
		}

		[[nodiscard]]
		constexpr const handle_type&& GetHandle() const&& noexcept
		{
			return std::move(myHandle);
		}

		explicit constexpr operator handle_type& () & noexcept
		{
			return myHandle;
		}

		explicit constexpr operator const handle_type& () const& noexcept
		{
			return myHandle;
		}

		explicit constexpr operator handle_type && () && noexcept
		{
			return std::move(myHandle);
		}

		explicit constexpr operator const handle_type && () const&& noexcept
		{
			return std::move(myHandle);
		}

		[[nodiscard]]
		constexpr bool operator==(const IHandle& other) const noexcept
		{
			return myHandle == other.myHandle;
		}

		[[nodiscard]]
		constexpr bool operator==(const handle_type& handle) const noexcept
		{
			return myHandle == handle;
		}

		constexpr IHandle(const IHandle&)
			noexcept(util::nothrow_copy_constructibles<handle_type>)
			requires(util::copyable<handle_type>) = default;
		constexpr IHandle(IHandle&&)
			noexcept(util::nothrow_move_constructibles<handle_type>) = default;
		constexpr IHandle& operator=(const IHandle&)
			noexcept(util::nothrow_copy_assignables<handle_type>)
			requires(util::copyable<handle_type>) = default;
		constexpr IHandle& operator=(IHandle&&)
			noexcept(util::nothrow_move_assignables<handle_type>) = default;

		handle_type myHandle;
	};
}
