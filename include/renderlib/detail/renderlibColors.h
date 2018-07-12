#pragma once

namespace _detail
{
	template <typename T>
	struct RGB
	{
		T r;
		T g;
		T b;

		RGB() :
			r((T)0), g((T)0), b((T)0)
		{}

		RGB(T _r, T _g, T _b) :
			r(_r), g(_g), b(_b)
		{}
	};

	template <typename T>
	struct RGBA
	{
		T r;
		T g;
		T b;
		T a;

		RGBA() :
			r(255), g(255), b(255), a(255)
		{};

		RGBA(T RGB, T A = 255) :
			r(RGB), g(RGB), b(RGB), a(A)
		{}

		RGBA(T R, T G, T B, T A = 255) :
			r(R), g(G), b(B), a(A)
		{};

		RGBA(const RGBA& x) :
			r(x.r), g(x.g), b(x.b), a(x.a)
		{}

		RGBA(const _detail::RGB<T>& x, T a = (T)255) :
			r(x.r), g(x.g), b(x.b), a(a)
		{}

		RGBA& operator=(const RGBA& x)
		{
			r = x.r;
			g = x.g;
			b = x.b;
			a = x.a;

			return *this;
		}
	};
}

using RGB = _detail::RGB<uint8_t>;
using RGBf = _detail::RGB<float>;

using RGBA = _detail::RGBA<uint8_t>;