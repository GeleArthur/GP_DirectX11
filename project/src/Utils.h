#pragma once
#include <fstream>
#include <vector>


struct Vertex_PosTexture;

namespace Utils
{
	bool ParseOBJ(const std::string& filename, std::vector<Vertex_PosTexture>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding = true);

	/* --- CONSTANTS --- */
	constexpr auto PI = 3.14159265358979323846f;
	constexpr auto PI_DIV_2 = 1.57079632679489661923f;
	constexpr auto PI_DIV_4 = 0.785398163397448309616f;
	constexpr auto PI_2 = 6.283185307179586476925f;
	constexpr auto PI_4 = 12.56637061435917295385f;

	constexpr auto TO_DEGREES = (180.0f / PI);
	constexpr auto TO_RADIANS(PI / 180.0f);

	/* --- HELPER FUNCTIONS --- */
	template<typename  T>
	float Square(const T a)
	{
		return a * a;
	}

	template<typename  T>
	float Lerpf(const T a,const T b,const T factor)
	{
		return ((1 - factor) * a) + (factor * b);
	}

	template<typename  T>
	bool AreEqual(T a, T b, float epsilon = FLT_EPSILON)
	{
		return abs(a - b) < epsilon;
	}

	template<typename  T>
	int Clamp(const T v, const T min, const T max)
	{
		if (v < min) return min;
		if (v > max) return max;
		return v;
	}
	template<typename T>
	float Clamp(const T v, const T min, const T max)
	{
		if (v < min) return min;
		if (v > max) return max;
		return v;
	}

	template<typename T>
	float Saturate(const T v)
	{
		if (v < 0.f) return 0.f;
		if (v > 1.f) return 1.f;
		return v;
	}


}
