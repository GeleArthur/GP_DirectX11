#pragma once
#include "Vector.h"

template<class T>
struct Matrix
{
	Matrix() = default;

	explicit  Matrix(
		const Vector<3, T>& xAxis,
		const Vector<3, T>& yAxis,
		const Vector<3, T>& zAxis,
		const Vector<3, T>& t):
		data{
			{xAxis, 0},
			{yAxis, 0},
			{zAxis, 0},
			{t, 1}}
	{}

	explicit  Matrix(
		const Vector<4,T>& xAxis,
		const Vector<4,T>& yAxis,
		const Vector<4,T>& zAxis,
		const Vector<4,T>& t):
	data{xAxis, yAxis, zAxis, t}{}

	Vector<3,T> TransformVector(const Vector<3,T>& v) const
	{
		return TransformVector(v.x, v.y, v.z);
	}
	Vector<3,T> TransformVector(T x, T y, T z) const
	{
		return Vector<3,T>{
			data[0].x * x + data[1].x * y + data[2].x * z,
			data[0].y * x + data[1].y * y + data[2].y * z,
			data[0].z * x + data[1].z * y + data[2].z * z
		};
	}
	Vector<3,T> TransformPoint(const Vector<3,T>& p) const
	{
		return TransformPoint(p.x, p.y, p.z);
	}
	Vector<3,T> TransformPoint(T x, T y, T z) const
	{
		return Vector<3,T>{
			data[0].x * x + data[1].x * y + data[2].x * z + data[3].x,
			data[0].y * x + data[1].y * y + data[2].y * z + data[3].y,
			data[0].z * x + data[1].z * y + data[2].z * z + data[3].z,
		};
	}

	Vector<4,T> TransformPoint(const Vector<4,T>& p) const
	{
		return TransformPoint(p.x, p.y, p.z, p.w);
	}
	Vector<4,T> TransformPoint(T x, T y, T z, T w) const
	{
		return Vector<4,T>{
			data[0].x * x + data[1].x * y + data[2].x * z + data[3].x,
			data[0].y * x + data[1].y * y + data[2].y * z + data[3].y,
			data[0].z * x + data[1].z * y + data[2].z * z + data[3].z,
			data[0].w * x + data[1].w * y + data[2].w * z + data[3].w
		};
	}

	const Matrix& Transpose()
	{
		Matrix result{};
		for (int r{ 0 }; r < 4; ++r)
		{
			for (int c{ 0 }; c < 4; ++c)
			{
				result[r][c] = data[c][r];
			}
		}

		data[0] = result[0];
		data[1] = result[1];
		data[2] = result[2];
		data[3] = result[3];

		return *this;
	}
	const Matrix& Inverse()
	{
		//Optimized Inverse as explained in FGED1 - used widely in other libraries too.
		const Vector<3,T>& a = Vector<3,T>{data[0]};
		const Vector<3,T>& b = Vector<3,T>{data[1]};
		const Vector<3,T>& c = Vector<3,T>{data[2]};
		const Vector<3,T>& d = Vector<3,T>{data[3]};

		const T x = data[0][3];
		const T y = data[1][3];
		const T z = data[2][3];
		const T w = data[3][3];

		Vector<3,T> s = Vector<3,T>::Cross(a, b);
		Vector<3,T> t = Vector<3,T>::Cross(c, d);
		Vector<3,T> u = a * y - b * x;
		Vector<3,T> v = c * w - d * z;

		T det = Vector<3,T>::Dot(s, v) + Vector<3,T>::Dot(t, u);
		//assert((!AreEqual(det, 0.f)) && "ERROR: determinant is 0, there is no INVERSE!");
		T invDet = 1.f / det;

		s *= invDet; t *= invDet; u *= invDet; v *= invDet;

		Vector<3,T> r0 = Vector<3,T>::Cross(b, v) + t * y;
		Vector<3,T> r1 = Vector<3,T>::Cross(v, a) - t * x;
		Vector<3,T> r2 = Vector<3,T>::Cross(d, u) + s * w;
		Vector<3,T> r3 = Vector<3,T>::Cross(u, c) - s * z;

		data[0] = Vector<4,T>{ r0.x, r1.x, r2.x, 0.f };
		data[1] = Vector<4,T>{ r0.y, r1.y, r2.y, 0.f };
		data[2] = Vector<4,T>{ r0.z, r1.z, r2.z, 0.f };
		data[3] = { -Vector<3,T>::Dot(b, t), Vector<3,T>::Dot(a, t),-Vector<3,T>::Dot(d, s),Vector<3,T>::Dot(c, s) };

		return *this;
	}

	Vector<3,T> GetAxisX() const
	{
		return data[0];
	}
	Vector<3,T> GetAxisY() const
	{
		return data[1];
	}
	Vector<3,T> GetAxisZ() const
	{
		return data[2];
	}
	Vector<3,T> GetTranslation() const
	{
		return Vector3{data[0][3], data[1][3], data[2][3]};
	}

	static Matrix CreateTranslation(T x, T y, T z)
	{
		return CreateTranslation({ x, y, z });
	}
	static Matrix CreateTranslation(const Vector<3,T>& t)
	{
		return { Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, t };
	}
	static Matrix CreateRotationX(T pitch)
	{
		return Matrix{
			{1, 0, 0, 0},
			{0, cos(pitch), -sin(pitch), 0},
			{0, sin(pitch), cos(pitch), 0},
			{0, 0, 0, 1}
		};
	}
	static Matrix CreateRotationY(T yaw)
	{
		return Matrix{
			{cos(yaw), 0, -sin(yaw), 0},
			{0, 1, 0, 0},
			{sin(yaw), 0, cos(yaw), 0},
			{0, 0, 0, 1}
		};
	}
	static Matrix CreateRotationZ(T roll)
	{
		return Matrix{
			{cos(roll), sin(roll), 0, 0},
			{-sin(roll), cos(roll), 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		};
	}
	static Matrix CreateRotation(T pitch, T yaw, T roll)
	{
		return CreateRotation({ pitch, yaw, roll });
	}
	static Matrix CreateRotation(const Vector<3,T>& r)
	{
		return CreateRotationX(r[0]) * CreateRotationY(r[1]) * CreateRotationZ(r[2]);
	}
	static Matrix CreateScale(T sx, T sy, T sz)
	{
		return { {sx, 0, 0}, {0, sy, 0}, {0, 0, sz}, Vector3::Zero };
	}
	static Matrix CreateScale(const Vector<3,T>& s)
	{
		return CreateScale(s[0], s[1], s[2]);
	}
	static Matrix Transpose(const Matrix& m)
	{
		Matrix out{ m };
		out.Transpose();

		return out;
	}
	static Matrix Inverse(const Matrix& m)
	{
		Matrix out{ m };
		out.Inverse();

		return out;
	}

	static Matrix CreatePerspectiveFovLH(T fov, T aspect, T neart, T fart)
	{
		return Matrix{
			{1.0f/(aspect*fov),0       ,0                     ,0},
			{0                ,1.0f/fov,0                     ,0},
			{0                ,0       ,fart/(fart-neart)        ,1.0f},
			{0                ,0       ,(- (fart * neart)) / (fart - neart),0}
		};
	}

	Vector<4,T>& operator[](int index)
	{
		assert(index <= 3 && index >= 0);
		return data[index];
	}
	Vector<4,T> operator[](int index) const
	{
		assert(index <= 3 && index >= 0);
		return data[index];
	}
	Matrix operator*(const Matrix& m) const
	{
		Matrix result{};
		Matrix m_transposed = Transpose(m);

		for (int r{ 0 }; r < 4; ++r)
		{
			for (int c{ 0 }; c < 4; ++c)
			{
				result[r][c] = Vector<4,T>::Dot(data[r], m_transposed[c]);
			}
		}

		return result;
	}
	const Matrix& operator*=(const Matrix& m)
	{
		Matrix copy{ *this };
		Matrix m_transposed = Transpose(m);

		for (int r{ 0 }; r < 4; ++r)
		{
			for (int c{ 0 }; c < 4; ++c)
			{
				data[r][c] = Vector<4,T>::Dot(copy[r], m_transposed[c]);
			}
		}

		return *this;
	}
	bool operator==(const Matrix& m) const
	{
		return data[0] == m.data[0]
		&& data[1] == m.data[1]
		&& data[2] == m.data[2]
		&& data[3] == m.data[3];
	}

private:

	//Row-Major Matrix
	Vector<4,T> data[4]
	{
		Vector<4,T>{1,0,0,0}, //xAxis
		Vector<4,T>{0,1,0,0}, //yAxis
		Vector<4,T>{0,0,1,0}, //zAxis
		Vector<4,T>{0,0,0,1}  //T
	};

	// v0x v0y v0z v0w
	// v1x v1y v1z v1w
	// v2x v2y v2z v2w
	// v3x v3y v3z v3w
};
