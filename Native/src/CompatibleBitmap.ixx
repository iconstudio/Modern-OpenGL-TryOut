export module Glib.Device.Resource.CompatibleBitmap;
import <type_traits>;
import Glib.Definitions;
import Glib.Device.IHandle;
import Glib.Device.Resource.Bitmap;

export namespace gl::device::resource
{
	class [[nodiscard]] CompatibleBitmap : public Bitmap
	{
	public:
		using base = Bitmap;

		CompatibleBitmap(handle_type&& handle) noexcept;
		~CompatibleBitmap() noexcept;

		explicit constexpr operator handle_type& () & noexcept
		{
			return GetHandle();
		}

		explicit constexpr operator const handle_type& () const& noexcept
		{
			return GetHandle();
		}

		explicit constexpr operator handle_type && () && noexcept
		{
			return std::move(GetHandle());
		}

		explicit constexpr operator const handle_type && () const&& noexcept
		{
			return std::move(GetHandle());
		}

		CompatibleBitmap(const CompatibleBitmap&) = delete;
		constexpr CompatibleBitmap(CompatibleBitmap&&) noexcept = default;
		CompatibleBitmap& operator=(const CompatibleBitmap&) = delete;
		constexpr CompatibleBitmap& operator=(CompatibleBitmap&&) noexcept = default;
	};
}
