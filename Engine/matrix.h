#pragma once

#include "vector.h"
#include "sphere.h"
//*
template<typename type,int nComp>
class SquareMatrix
{
protected:
	type m[nComp*nComp];
public:
	typedef Vector<type,nComp> Point;
	typedef SquareMatrix<type,nComp> Matrix;

	SquareMatrix() {}

	operator type*() { return m; }
	operator const type*() const { return m; }

	void Zero();
	void Identity();
	void GetTranspose(Matrix& oDest) const;

	void GetRotatePart(SquareMatrix<type,nComp-1>& oDest) const;

	void Multiply(const Matrix& a,const Matrix& b);

	Vector<type,nComp> Transform(const Vector<type,nComp>& iVec) const;
	Sphere<type,nComp-1> TransformSphere(const Sphere<type,nComp-1>& iVec) const;

	Point GetRow(int i) const { Point r; for(int j=0;j<nComp;j++) r[j] = m[i*nComp+j]; return r; }
	void GetRow(int i,Point& d) const { for(int j=0;j<nComp;j++) d[j] = m[i*nComp+j];}
	void SetRow(int i,const Point& p) { for(int j=0;j<nComp;j++) m[i*nComp+j] = p[j];}
	Point GetCol(int i) const { Point r; for(int j=0;j<nComp;j++) r[j] = m[i+nComp*j]; return r; }
	void GetCol(int i,Point& d) const { for(int j=0;j<nComp;j++) d[j] = m[i+nComp*j];}
	void SetCol(int i,const Point& p) { for(int j=0;j<nComp;j++) m[i+nComp*j] = p[j];}

	Matrix& operator+=(const Matrix& b) { for(int i=0;i<nComp*nComp;i++) m[i] += b[i]; return *this; }
	Matrix& operator/=(const float& b) { for(int i=0;i<nComp*nComp;i++) m[i] /= b; return *this; }
	//*/
};

template<typename type>
class Matrix2 : public SquareMatrix<type,2>
{
	using SquareMatrix<type,2>::m;
public:
	typedef SquareMatrix<type,2> Matrix;
	typedef Vector<type,2> Point;

	bool GetInvert(Matrix& oDest) const;

	void Rotate(type alpha) { Rotate(type(cos(alpha)),type(sin(alpha))); }
	void Rotate(type cosa,type sina);

	Point GetRg() const { return SquareMatrix<type,2>::GetRow(0); }
	Point GetUp() const { return SquareMatrix<type,2>::GetRow(1); }
	void GetRg(Point& d) const { SquareMatrix<type,2>::GetRow(0,d); }
	void GetUp(Point& d) const { SquareMatrix<type,2>::GetRow(1,d); }
	void SetRg(const Point& p) { SquareMatrix<type,2>::SetRow(0,p); }
	void SetUp(const Point& p) { SquareMatrix<type,2>::SetRow(1,p); }
};

template<typename type>
class Matrix3 : public SquareMatrix<type,3>
{
	using SquareMatrix<type,3>::m;
public:
	typedef SquareMatrix<type,3> Matrix;
	typedef Vector<type,3> Point;

	Sphere<type,2> TransformCircle(const Sphere<type,2>& iVec) const;

	bool GetInvert(Matrix& oDest) const;

	void SetLookAt(const Vector<type,2>& p,const Vector<type,2>& l);

	// like transform 2D
	void Rotate(type alpha) { RotateZ(type(cos(alpha)),type(sin(alpha))); }
	void Rotate(type cosa,type sina) { RotateZ(cosa,sina); }

	// like rotation 3D
	void RotateX(float alpha) { RotateX(cosf(alpha),sinf(alpha)); }
	void RotateY(float alpha) { RotateY(cosf(alpha),sinf(alpha)); }
	void RotateZ(float alpha) { RotateZ(cosf(alpha),sinf(alpha)); }
	void RotateAxis(const Point& axis,float alpha) { RotateAxis(axis,cosf(alpha),sinf(alpha)); }

	void RotateX(float cosa,float sina);
	void RotateY(float cosa,float sina);
	void RotateZ(float cosa,float sina);
	void RotateAxis(const Vector<type,3>& axis,float cosa,float sina);

	Point GetRg() const { return SquareMatrix<type,3>::GetRow(0); }
	Point GetUp() const { return SquareMatrix<type,3>::GetRow(1); }
	Point GetAt() const { return SquareMatrix<type,3>::GetRow(2); } //like rotation 3D
	Point GetPos() const { return SquareMatrix<type,3>::GetRow(2); } // like transform 2D
	void GetRg(Point& d) const { SquareMatrix<type,3>::GetRow(0,d); }
	void GetUp(Point& d) const { SquareMatrix<type,3>::GetRow(1,d); }
	void GetAt(Point& d) const { SquareMatrix<type,3>::GetRow(2,d); } //like rotation 3D
	void GetPos(Point& d) const { SquareMatrix<type,3>::GetRow(2,d); } // like transform 2D
	void SetRg(const Point& p) { SquareMatrix<type,3>::SetRow(0,p); }
	void SetUp(const Point& p) { SquareMatrix<type,3>::SetRow(1,p); }
	void SetAt(const Point& p) { SquareMatrix<type,3>::SetRow(2,p); } //like rotation 3D
	void SetPos(const Point& p) { SquareMatrix<type,3>::SetRow(2,p); } // like transform 2D
};

template<typename type>
class Matrix4 : public SquareMatrix<type,4>
{
	using SquareMatrix<type,4>::m;
public:
	typedef SquareMatrix<type,4> Matrix;
	typedef Vector<type,4> Point;

	bool GetInvert(Matrix& oDest) const;

	void SetLookAt(const Vector<type,3>& p,const Vector<type,3>& l);

	void RotateX(float alpha) { RotateX(cosf(alpha),sinf(alpha)); }
	void RotateY(float alpha) { RotateY(cosf(alpha),sinf(alpha)); }
	void RotateZ(float alpha) { RotateZ(cosf(alpha),sinf(alpha)); }
	void RotateAxis(const Vector<type,3>& axis,float alpha) { RotateAxis(axis,cosf(alpha),sinf(alpha)); }

	void RotateX(float cosa,float sina);
	void RotateY(float cosa,float sina);
	void RotateZ(float cosa,float sina);
	void RotateAxis(const Vector<type,3>& axis,float cosa,float sina);

	Point GetRg() const { return SquareMatrix<type,4>::GetRow(0); }
	Point GetUp() const { return SquareMatrix<type,4>::GetRow(1); }
	Point GetAt() const { return SquareMatrix<type,4>::GetRow(2); }
	Point GetPos() const { return SquareMatrix<type,4>::GetRow(3); }
	void GetRg(Point& d) const { SquareMatrix<type,4>::GetRow(0,d); }
	void GetUp(Point& d) const { SquareMatrix<type,4>::GetRow(1,d); }
	void GetAt(Point& d) const { SquareMatrix<type,4>::GetRow(2,d); }
	void GetPos(Point& d) const { SquareMatrix<type,4>::GetRow(3,d); }
	void SetRg(const Point& p) { SquareMatrix<type,4>::SetRow(0,p); }
	void SetUp(const Point& p) { SquareMatrix<type,4>::SetRow(1,p); }
	void SetAt(const Point& p) { SquareMatrix<type,4>::SetRow(2,p); }
	void SetPos(const Point& p) { SquareMatrix<type,4>::SetRow(3,p); }
};
//*/
template<typename type,int nComp>
SquareMatrix<type,nComp> operator+(const SquareMatrix<type,nComp> &a,const SquareMatrix<type,nComp> &b);

#include "matrix.inl"

typedef Matrix2<float> Matrix2f;
typedef Matrix3<float> Matrix3f;
typedef Matrix4<float> Matrix4f;
typedef Matrix2<double> Matrix2d;
typedef Matrix3<double> Matrix3d;
typedef Matrix4<double> Matrix4d;
