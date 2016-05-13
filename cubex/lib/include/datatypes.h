#ifndef _DATATYPES_H_
#define _DATATYPES_H_

#include "common.h"

template<class T>
inline bool CmpReal(T a, T b, T eps = T(0.0001)) {
	return (T)fabs(double(a - b)) <= eps;
}

template<class T> union Vector3;
template<class T> union Vector4;
template<class T> union Matrix33;
template<class T> union Matrix44;

template<class T>
union Vector2
{
	T data[2];
	struct {
		T x, y;
	};

	explicit Vector2(T init = T(0)) { x = y = init; }
	Vector2(T x, T y) {
		this->x = x; this->y = y;
	}
	
	template<class T2> operator Vector2<T2>() const {
		return Vector2<T2>(T2(x), T2(y));
	}
	template<class T2> operator Vector3<T2>() const {
		return Vector3<T2>(T2(x), T2(y), T2(0));
	}
	template<class T2> operator Vector4<T2>() const {
		return Vector4<T2>(T2(x), T2(y), T2(0), T2(1));
	}

	T Length() const;
	T LengthSquared() const;
	void Normalize();

	T &operator[](int i) { return data[i]; }
	const T &operator[](int i) const { return data[i]; }

	bool operator==(const Vector2 &v) const;
	bool operator!=(const Vector2 &v) const;

	Vector2 operator+(const Vector2 &v) const;
	Vector2 operator-(const Vector2 &v) const;
	Vector2 operator-() const;
	Vector2 operator*(T scale) const;
	Vector2 operator/(T scale) const;

	Vector2 &operator+=(const Vector2 &v);
	Vector2 &operator-=(const Vector2 &v);
	Vector2 &operator*=(T scale);
	Vector2 &operator/=(T scale);
};

template<class T>
union Vector3
{
	T data[3];
	struct {
		T x, y, z;
	};
	struct {
		T r, g, b;
	};

	explicit Vector3(T init = T(0)) { x = y = z = init; }
	Vector3(T x, T y, T z) {
		this->x = x; this->y = y; this->z = z;
	}

	template<class T2> operator Vector2<T2>() const {
		return Vector2<T2>(T2(x), T2(y));
	}
	template<class T2> operator Vector3<T2>() const {
		return Vector3<T2>(T2(x), T2(y), T2(z));
	}
	template<class T2> operator Vector4<T2>() const {
		return Vector4<T2>(T2(x), T2(y), T2(z), T2(1));
	}

	T Length() const;
	T LengthSquared() const;
	void Normalize();

	T &operator[](int i) { return data[i]; }
	const T &operator[](int i) const { return data[i]; }

	bool operator==(const Vector3 &v) const;
	bool operator!=(const Vector3 &v) const;

	Vector3 operator+(const Vector3 &v) const;
	Vector3 operator-(const Vector3 &v) const;
	Vector3 operator-() const;
	Vector3 operator*(T scale) const;
	Vector3 operator*(const Matrix33<T> &m) const;
	Vector3 operator/(T scale) const;

	Vector3 &operator+=(const Vector3 &v);
	Vector3 &operator-=(const Vector3 &v);
	Vector3 &operator*=(T scale);
	Vector3 &operator*=(const Matrix33<T> &m);
	Vector3 &operator/=(T scale);
};

template<class T>
union Vector4
{
	T data[4];
	struct {
		T x, y, z, w;
	};
	struct {
		T r, g, b, a;
	};

	Vector4() { x = y = z = T(0); w = T(1); }
	explicit Vector4(T init) { x = y = z = w = init; }
	Vector4(T x, T y, T z, T w = T(1)) {
		this->x = x; this->y = y; this->z = z; this->w = w;
	}

	template<class T2> operator Vector2<T2>() const {
		return Vector2<T2>(T2(x), T2(y));
	}
	template<class T2> operator Vector3<T2>() const {
		return Vector3<T2>(T2(x), T2(y), T2(z));
	}
	template<class T2> operator Vector4<T2>() const {
		return Vector4<T2>(T2(x), T2(y), T2(z), T2(w));
	}

	T Length() const;
	T LengthSquared() const;
	void Normalize();
	void Cartesian();

	T &operator[](int i) { return data[i]; }
	const T &operator[](int i) const { return data[i]; }

	bool operator==(const Vector4 &v) const;
	bool operator!=(const Vector4 &v) const;

	Vector4 operator+(const Vector4 &v) const;
	Vector4 operator-(const Vector4 &v) const;
	Vector4 operator-() const;
	Vector4 operator*(T scale) const;
	Vector4 operator*(const Matrix44<T> &m) const;
	Vector4 operator/(T scale) const;
		  
	Vector4 &operator+=(const Vector4 &v);
	Vector4 &operator-=(const Vector4 &v);
	Vector4 &operator*=(T scale);
	Vector4 &operator*=(const Matrix44<T> &m);
	Vector4 &operator/=(T scale);
};

template<class T>
union Matrix33
{
	T data[9];
	T m[3][3];

	struct {
		Vector3<T> xAxis, yAxis, zAxis;
	};

	Matrix33() {
		data[0] = data[4] = data[8] = T(1);
	}
	explicit Matrix33(T diag) {
		data[0] = data[4] = data[8] = diag;
	}
	Matrix33(const T m[9]);
	template<class T2> explicit Matrix33(const Matrix33<T2> &m);
	template<class T2> explicit Matrix33(const Matrix44<T2> &m);

	void Scale(T factor);
	T Determinant() const;
	Matrix33 GetInverse() const;
	Matrix33 GetTranspose() const;
	
	void LoadIdentity();
	static Matrix33 Identity();
	static Matrix33 Multiply(const Matrix33 &m1, const Matrix33 &m2);
	static Matrix33 Multiply(const Matrix33 &m, const Vector3<T> &v);

	bool operator==(const Matrix33 &m) const;
	bool operator!=(const Matrix33 &m) const;
	Vector3<T> operator*(const Vector3<T> &v) const;
	Matrix33 operator*(const Matrix33 &m) const;
	Matrix33 &operator*=(const Matrix33 &m);
};

template<class T>
union Matrix44
{
	T data[16];
	T m[4][4];

	struct {
		Vector3<T> xAxis; T wx;
		Vector3<T> yAxis; T wy;
		Vector3<T> zAxis; T wz;
		Vector3<T> translate; T wt;
	};

	Matrix44() {
		wx = wy = wz = T(0);
		data[0] = data[5] = data[10] = data[15] = T(1);
	}
	explicit Matrix44(T diag) {
		wx = wy = wz = T(0);
		data[0] = data[5] = data[10] = data[15] = diag;
	}
	Matrix44(const T m[16]);
	template<class T2> explicit Matrix44(const Matrix33<T2> &m);
	template<class T2> explicit Matrix44(const Matrix44<T2> &m);

	void Scale(T factor);
	void SetRotation(const Matrix33<T> &m);

	T Determinant() const;
	Matrix44 GetTranspose() const;
	Matrix44 GetInverse() const;

	void LoadIdentity();
	static Matrix44 Identity();
	static Matrix44 Multiply(const Matrix44 &m1, const Matrix44 &m2);

	bool operator==(const Matrix44 &m) const;
	bool operator!=(const Matrix44 &m) const;
	Vector4<T> operator*(const Vector4<T> &v) const;
	Matrix44 operator*(const Matrix44 &m) const;
	Matrix44 &operator*=(const Matrix44 &m);
};

typedef Vector2<int>     Vector2i,  Point2i;
typedef Vector3<int>     Vector3i,  Point3i;
typedef Vector4<int>     Vector4i,  Point4i;

typedef Vector2<float>   Vector2f,  Point2f;
typedef Vector3<float>   Vector3f,  Point3f, Color3f;
typedef Vector4<float>   Vector4f,  Point4f, Color4f;
typedef Matrix33<float>  Matrix33f;
typedef Matrix44<float>  Matrix44f;

typedef Vector2<double>  Vector2d,  Point2d;
typedef Vector3<double>  Vector3d,  Point3d, Color3d;
typedef Vector4<double>  Vector4d,  Point4d, Color4d;
typedef Matrix33<double> Matrix33d;
typedef Matrix44<double> Matrix44d;

typedef Vector3<unsigned char> Color3b;
typedef Vector4<unsigned char> Color4b;

template<class T>
T Dot(Vector3<T> v1, Vector3<T> v2) {
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

template<class T>
T Dot(Vector4<T> v1, Vector4<T> v2) {
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
}

template<class T>
Vector3<T> Cross(Vector3<T> v1, Vector3<T> v2) {
	return Vector3<T>(
		v1.y*v2.z - v1.z*v2.y,
		v1.z*v2.x - v1.x*v2.z,
		v1.x*v2.y - v1.y*v2.x);
}

template<class T>
Vector3<T> Normalize(Vector3<T> v) {
	T f = T(1) / v.Length();
	return Vector3<T>(v.x*f, v.y*f, v.z*f);
}

template<class T>
Vector4<T> Normalize(Vector4<T> v) {
	T f = T(1) / v.Length();
	return Vector4<T>(v.x*f, v.y*f, v.z*f, v.w*f);
}

#include "datatypes.inl"

#endif // _DATATYPES_H_