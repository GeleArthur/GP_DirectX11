#pragma once
#include <cassert>
#include <cstdint>



template<uint32_t count, typename T>
struct Vector
{
};

template<typename T>
struct Vector<2, T>
{
	T x;
	T y;

	Vector() = default;
	Vector(const T& tx, const T& ty) : x{tx}, y{ty} {}

	T Magnitude() const
	{
		return sqrtf(x * x + y * y);
	}

	T SqrMagnitude() const
	{
		return x * x + y * y;
	}

	Vector<2,T> Normalized() const
	{
		T magnitude = Magnitude();
		return {x / magnitude, y / magnitude};
	}

	static T Dot(const Vector& v1, const Vector& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}
	static T Cross(const Vector& v1, const Vector& v2)
	{
		return v1.x * v2.y - v1.y * v2.x;
	}

	Vector operator*(T scale) const
	{
		return {x * scale, y * scale};
	}
	
	Vector operator/(T scale) const
	{
		return {x / scale, y / scale};
	}
	Vector operator+(const Vector& v) const
	{
		return {x + v.x, y + v.y};
	}
	Vector operator-(const Vector& v) const
	{
		return {x - v.x, y - v.y};
	}
	Vector operator-() const
	{
		return {-x, -y};
	}

	Vector& operator+=(const Vector& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector& operator-=(const Vector& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vector& operator/=(T scale)
	{
		x /= scale;
		y /= scale;
		return *this;
	}
	Vector& operator*=(T scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}

	T& operator[](int index)
	{
		// assert(index <= 1 && index >= 0);
		return index == 0 ? x : y;
	}
	T operator[](int index) const
	{
		//assert(index <= 1 && index >= 0);
		return index == 0 ? x : y;
	}

	bool operator==(const Vector& v) const
	{
		return x == v.x && y == v.y;
	}

	

};

template<typename T>
struct Vector<3, T>
{
	T x;
	T y;
	T z;

	Vector() = default;
	Vector(const T& tx, const T& ty, const T& tz) : x{tx}, y{ty}, z{tz} {}
	explicit  Vector(const Vector<4,T>& v) : x(v.x), y(v.y), z(v.z){}

	T Magnitude() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	T SqrMagnitude() const
	{
		return x * x + y * y + z * z;
	}

	Vector Normalized() const
	{
		T magnitude = Magnitude();
		return {x / magnitude, y / magnitude, z / magnitude};
	}

	T Normalize()
	{
		T magnitude = Magnitude();
		*this /= magnitude;
		
		return magnitude;
	}
	
	static T Dot(const Vector& v1, const Vector& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}
	
	static Vector Cross(const Vector& v1, const Vector& v2)
	{
		return {
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x
		};
	}

	static Vector Reject(const Vector& v1, const Vector& v2)
	{
		return (v1 - v2 * (Dot(v1, v2) / Dot(v2, v2)));
	}

	static Vector Reflect(const Vector& v1, const Vector& v2)
	{
		return v1 - (v2 * (2.f * Dot(v1, v2)));
	}

	Vector operator*(T scale) const
	{
		return {x * scale, y * scale, z * scale};
	}
	Vector operator/(T scale) const
	{
		return {x / scale, y / scale, z / scale};
	}
	Vector operator+(const Vector& v) const
	{
		return {x + v.x, y + v.y, z + v.z};
	}
	Vector operator-(const Vector& v) const
	{
		return {x - v.x, y - v.y, z - v.z};
	}
	Vector operator-() const
	{
		return {-x, -y, -z};
	}

	Vector& operator+=(const Vector& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector& operator-=(const Vector& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector& operator/=(T scale)
	{
		x /= scale;
		y /= scale;
		z /= scale;
		return *this;
	}
	Vector& operator*=(T scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}

	bool operator==(const Vector& v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	T& operator[](int index)
	{
		assert(index <= 2 && index >= 0);

		if (index == 0)return x;
		if (index == 1)return y;
		return z;
	}

	T operator[](int index) const
	{
		assert(index <= 2 && index >= 0);

		if (index == 0)return x;
		if (index == 1)return y;
		return z;
	}

	static const Vector UnitX;
	static const Vector UnitY;
	static const Vector UnitZ;
	static const Vector Zero;
};

template<typename T>
Vector<3,T> const Vector<3,T>::UnitX{1,0,0};

template<typename T>
Vector<3,T> const Vector<3,T>::UnitY{0,1,0};

template<typename T>
Vector<3,T> const Vector<3,T>::UnitZ{0,0,1};

template<typename T>
Vector<3,T> const Vector<3,T>::Zero{0,0,0};

template<typename T>
Vector<3,T> operator*(T scale, const Vector<3,T>& v)
{
	return {v.x * scale, v.y * scale, v.z * scale };
}

template<typename T>
struct Vector<4, T>
{
	T x;
	T y;
	T z;
	T w;

	Vector() = default;
	Vector(const T& tx, const T& ty, const T& tz, const T& tw) : x{tx}, y{ty}, z{tz}, w{tw} {}
	explicit  Vector(const Vector<3,T>& v, const T& _w) : x{v.x}, y{v.y}, z{v.z}, w{_w} {}

	T Magnitude() const
	{
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	T SqrMagnitude() const
	{
		return x * x + y * y + z * z + w * w;
	}

	Vector<2,T> Normalized() const
	{
		T magnitude = Magnitude();
		return {x / magnitude, y / magnitude, z / magnitude, w / magnitude};
	}

	static T Dot(const Vector& v1, const Vector& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}

	Vector operator*(T scale) const
	{
		return {x * scale, y * scale, z * scale, w * scale};
	}
	Vector operator/(T scale) const
	{
		return {x / scale, y / scale, z / scale, w / scale};
	}
	Vector operator+(const Vector& v) const
	{
		return {x + v.x, y + v.y, z + v.z, w + v.w};
	}
	Vector operator-(const Vector& v) const
	{
		return {x - v.x, y - v.y, z - v.z, w - v.w};
	}
	Vector& operator+=(const Vector& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	bool operator==(const Vector& v) const
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	T& operator[](int index)
	{
		assert(index <= 3 && index >= 0);

		if (index == 0)return x;
		if (index == 1)return y;
		if (index == 2)return z;
		return w;
	}

	T operator[](int index) const
	{
		assert(index <= 3 && index >= 0);

		if (index == 0)return x;
		if (index == 1)return y;
		if (index == 2)return z;
		return w;
	}
	
	static const Vector Zero;
};

template<typename T>
Vector<4,T> const Vector<4,T>::Zero{0,0,0, 0};


using Vector2 = Vector<2, float>;
using Vector3 = Vector<3, float>;
using Vector4 = Vector<4, float>;
