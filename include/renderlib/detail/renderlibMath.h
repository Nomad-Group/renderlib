#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef RGB

#include <stdint.h>
#include <string>

/*
* Math
*/
namespace _detail
{
	template <typename T>
	struct Vector2
	{
		T x;
		T y;

		Vector2() :
			x((T)0), y((T)0)
		{}

		Vector2(const T& _x, const T& _y) :
			x(_x), y(_y)
		{}

		Vector2(const Vector2<T>& other) :
			x(other.x, other.y)
		{}

		template <typename TOther>
		Vector2(const Vector2<TOther>& other) :
			x(static_cast<T>(other.x)), y(static_cast<T>(other.y))
		{}

		Vector2& operator=(const Vector2<T>& other)
		{
			x = other.x;
			y = other.y;
			return *this;
		}

		Vector2<T> operator+(const Vector2<T>& other) const
		{
			return Vector2(x + other.x, y + other.y);
		}
		Vector2<T>& operator+=(const Vector2<T>& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		Vector2<T> operator-(const Vector2<T>& other) const
		{
			return Vector2<T>(x - other.x, y - other.y);
		}
		Vector2<T>& operator-=(const Vector2<T>& other)
		{
			x -= other.x;
			y -= other.y;

			return *this;
		}

		Vector2<T> operator*(const T v) const
		{
			return Vector2<T>(x * v, y * v);
		}
		Vector2<T>& operator*=(const T v)
		{
			x *= v;
			y *= v;
			return *this;
		}

		bool operator==(const Vector2<T>& other)
		{
			return x == other.x && y == other.y;
		}
	};
}

using Vector2 = _detail::Vector2<int32_t>;
using Vector2f = _detail::Vector2<float>;

namespace _detail
{
	template <typename T>
	struct Rect
	{
		T x;
		T y;
		T w;
		T h;

		Rect() :
			x((T)0), y((T)0),
			w((T)0), h((T)0)
		{}

		Rect(T _x, T _y, T _w, T _h) :
			x(_x), y(_y),
			w(_w), h(_h)
		{}

		Rect(const _detail::Vector2<T>& position, const _detail::Vector2<T>& size) :
			x(position.x), y(position.y),
			w(size.x), h(size.y)
		{}

		inline bool Contains(const Rect& r) const
		{
			return
				r.x >= x && r.y >= y &&
				(r.x + r.w) <= (x + w) && (r.y + r.h) <= (y + h);
		}

		inline bool Contains(const _detail::Vector2<T>& pos, const _detail::Vector2<T>& size) const
		{
			return
				pos.x >= x && pos.y >= y &&
				(pos.x + size.x) <= (x + w) && (pos.y + size.y) <= (y + h);
		}

		inline bool IsIn(const Rect& r) const
		{
			return r.Contains(*this);
		}

		inline bool IsIn(const _detail::Vector2<T>& pos, const _detail::Vector2<T>& size) const
		{
			return
				x >= pos.x && y >= pos.y &&
				(x + w) <= (pos.x + size.x) && (y + h) <= (pos.y + size.y);
		}
	};
}

using Rect = _detail::Rect<int32_t>;
using Rectf = _detail::Rect<float>;