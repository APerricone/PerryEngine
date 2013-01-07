#pragma once

#include <math.h>

//Base Class
template<typename type,int nComp>
class Vector
{
protected:
	type v[nComp];
public:
	operator       type*()       { return v; }
	operator const type*() const { return v; }

	Vector(type _v = 0) { for(int i=0;i<nComp;i++) v[i] = _v; }
	Vector(const type* _v) { for(int i=0;i<nComp;i++) v[i] = _v[i]; }
	Vector<type,nComp> &Set(type _v = 0) { for(int i=0;i<nComp;i++) v[i] = _v; return *this; }

	Vector(const Vector<type,nComp-1> &_v,type _v1=0) { for(int i=0;i<nComp-1;i++) v[i] = _v[i]; v[nComp-1]=_v1; }
	//Vector(const Vector<type,nComp-2> &_v,type _v1=0,type _v2=0) { for(int i=0;i<nComp-2;i++) v[i] = _v; v[nComp-2]=_v1; v[nComp-1]=_v2; }
	void Set(const Vector<type,nComp-1> &_v,type _v1=0) { for(int i=0;i<nComp-1;i++) v[i] = _v[i]; v[nComp-1]=_v1; }
	//void Set(const Vector<type,nComp-2> &_v,type _v1=0,type _v2=0) { for(int i=0;i<nComp-2;i++) v[i] = _v; v[nComp-2]=_v1; v[nComp-1]=_v2; }

	Vector(type _x,type _y) 				{ v[0] = _x; v[1] = _y; 						for(int i=2;i<nComp;i++) v[i] = 0; }
	Vector(type _x,type _y,type _z) 		{ v[0] = _x; v[1] = _y; v[2] = _z; 				for(int i=3;i<nComp;i++) v[i] = 0; }
	Vector(type _x,type _y,type _z,type _w) { v[0] = _x; v[1] = _y; v[2] = _z; v[3] = _w;	for(int i=4;i<nComp;i++) v[i] = 0; }

	void Set(type _x,type _y)					{ v[0] = _x; v[1] = _y; for(int i=2;i<nComp;i++) v[i] = 0; }
	void Set(type _x,type _y,type _z)			{ v[0] = _x; v[1] = _y; v[2] = _z; for(int i=3;i<nComp;i++) v[i] = 0; }
	void Set(type _x,type _y,type _z,type _w)	{ v[0] = _x; v[1] = _y; v[2] = _z; v[3] = _w; for(int i=4;i<nComp;i++) v[i] = 0; }

	Vector<type,nComp>(const Vector<type,nComp> &b) { for(int i=0;i<nComp;i++) v[i] = b[i];  }
	Vector<type,nComp>& operator=(const Vector<type,nComp> &b) { for(int i=0;i<nComp;i++) v[i] = b[i]; return *this; }

	template<typename type2>
	Vector<type,nComp>(const Vector<type2,nComp> &b) { for(int i=0;i<nComp;i++) v[i] = static_cast<type>(b[i]); }
	template<typename type2>
	Vector<type,nComp>& operator=(const Vector<type2,nComp> &b) { for(int i=0;i<nComp;i++) v[i] = static_cast<type>(b[i]); return *this; }

	Vector<type,nComp>& operator+=(const Vector<type,nComp> &b) { for(int i=0;i<nComp;i++) v[i] += b[i]; return *this; }
	Vector<type,nComp>& operator-=(const Vector<type,nComp> &b) { for(int i=0;i<nComp;i++) v[i] -= b[i]; return *this; }
	Vector<type,nComp>& operator*=(const Vector<type,nComp> &b) { for(int i=0;i<nComp;i++) v[i] *= b[i]; return *this; }
	Vector<type,nComp>& operator/=(const Vector<type,nComp> &b) { for(int i=0;i<nComp;i++) v[i] /= b[i]; return *this; }
	Vector<type,nComp>& operator%=(const Vector<type,nComp> &b) { for(int i=0;i<nComp;i++) v[i] %= b[i]; return *this; }
	Vector<type,nComp>& operator+=(const type &b) { for(int i=0;i<nComp;i++) v[i] += b; return *this; }
	Vector<type,nComp>& operator-=(const type &b) { for(int i=0;i<nComp;i++) v[i] -= b; return *this; }
	Vector<type,nComp>& operator*=(const type &b) { for(int i=0;i<nComp;i++) v[i] *= b; return *this; }
	Vector<type,nComp>& operator/=(const type &b) { for(int i=0;i<nComp;i++) v[i] /= b; return *this; }
	Vector<type,nComp>& operator%=(const type &b) { for(int i=0;i<nComp;i++) v[i] %= b; return *this; }

	type GetLenSqr() const { return static_cast<type>(dot(*this,*this)); };
	type GetLen() const { return static_cast<type>(sqrt(static_cast<double>(dot(*this,*this)))); };
	type Normalize() { type l=GetLen(); *this/=l; return l; };
	Vector<type,nComp> GetNormalize() const { type l=GetLen(); return (*this)/l; };
	// defined below:
	// (*) operator+, operator-, operator*, operator/, between Vectors and with scalar
	// (*) dot product
};

// specializations

template<typename type>
class V2 : public Vector<type,2>
{
protected:
	using Vector<type,2>::v;
public:
	typedef Vector<type,2> base;
	V2(type x=0) : base(x) {}
	V2(type x,type y) : base(x,y) {}
	V2(const base &b) { for(int i=0;i<2;i++) base::v[i] = b[i]; }
	V2(const type* _v) : base(_v) {}

	void Set(type _v) { v[0] = v[1] = v[2] = _v; }
	void Set(type _x,type _y) { v[0] = _x; v[1] = _y; }

	type& x() { return base::v[0];  }
	type& y() { return base::v[1];  }
	const type& x() const { return base::v[0];  }
	const type& y() const { return base::v[1];  }

	type GetAngle() const { return static_cast<type>(atan2(base::v[1],base::v[0])); }

	V2<type>& operator=(const V2<type> &b) { for(int i=0;i<2;i++) base::v[i] = b[i]; return *this; }
};

template<typename type>
class V3 : public Vector<type,3>
{
protected:
	using Vector<type,3>::v;
public:
	typedef Vector<type,3> base;
	V3(type x=0) : base(x) {}
	V3(type x,type y,type z) : base(x,y,z) {}
	V3(const base &b) { for(int i=0;i<3;i++) base::v[i] = b[i]; }
	V3(const type* _v) : base(_v) {}
	V3(const Vector<type,2> &_v,type z=0) : base(_v,z) {}

	void Set(type _v) { v[0] = v[1] = v[2] = _v; }
	void Set(type _x,type _y,type _z=0) { v[0] = _x; v[1] = _y; v[2] = _z; }
	void Set(const Vector<type,2> &v,type z=0) { Set(v[0],v[1],z); }

	type& x() { return base::v[0];  }
	type& y() { return base::v[1];  }
	type& z() { return base::v[2];  }
	const type& x() const { return base::v[0];  }
	const type& y() const { return base::v[1];  }
	const type& z() const { return base::v[2];  }

	V3<type> & operator ^=(const V3<type>  &b) { V3<type>  t=*this; *this=t^b; return *this; }
};

template<typename type>
class V4 : public Vector<type,4>
{
protected:
	using Vector<type,4>::v;
public:
	typedef Vector<type,4> base;
	V4(type x=0) : base(x) {}
	V4(type x,type y,type z,type w) : base(x,y,z,w) {}
	V4(const base &b) { for(int i=0;i<4;i++) base::v[i] = b[i]; }
	V4(const type* _v) : base(_v) {}
	V4(const Vector<type,3> &_v,type w=0) : base(_v,w) {}
	V4(const Vector<type,2> &_v,type z=0,type w=0) : base(_v,z,w) {}

	void Set(type _v) { v[0] = v[1] = v[2] = v[3] = _v; }
	void Set(type _x,type _y,type _z=0,type _w=0) { v[0] = _x; v[1] = _y; v[2] = _z; v[3] = _w; }
	void Set(const Vector<type,2> &v,type z=0,type w=0) { Set(v[0],v[1],z,w); }
	void Set(const Vector<type,3> &v,type w=0) { Set(v[0],v[1],v[2],w); }

	type& x() { return base::v[0];  }
	type& y() { return base::v[1];  }
	type& z() { return base::v[2];  }
	type& w() { return base::v[3];  }
	const type& x() const { return base::v[0];  }
	const type& y() const { return base::v[1];  }
	const type& z() const { return base::v[2];  }
	const type& w() const { return base::v[3];  }

	V3<type> xyz() const { return V3<type>(v[0],v[1],v[2]); }
};

// predef
typedef V2<int> int2;
typedef V3<int> int3;
typedef V4<int> int4;
typedef V2<unsigned int> uint2;
typedef V3<unsigned int> uint3;
typedef V4<unsigned int> uint4;
typedef V2<long> long2;
typedef V3<long> long3;
typedef V4<long> long4;
typedef V2<unsigned long> ulong2;
typedef V3<unsigned long> ulong3;
typedef V4<unsigned long> ulong4;
typedef V2<long long> longlong2;
typedef V3<long long> longlong3;
typedef V4<long long> longlong4;
typedef V2<unsigned long long> ulonglong2;
typedef V3<unsigned long long> ulonglong3;
typedef V4<unsigned long long> ulonglong4;
typedef V2<float> float2;
typedef V3<float> float3;
typedef V4<float> float4;
typedef V2<double> double2;
typedef V3<double> double3;
typedef V4<double> double4;
// *************

// friend implementations

template <typename type,int nComp, typename f>
Vector<type,nComp> OpeatorTemplateVS(const Vector<type,nComp> &a,const type b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = f(a[i],b);
	return r;
}

template <typename type,int nComp, typename f>
Vector<type,nComp> OpeatorTemplateSV(const type a,const Vector<type,nComp> &b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = f(a,b[i]);
	return r;
}

// operators
template <typename type,int nComp>
Vector<type,nComp> operator+(const Vector<type,nComp> &a,const Vector<type,nComp> &b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a[i]+b[i];
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator-(const Vector<type,nComp> &a,const Vector<type,nComp> &b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a[i]-b[i];
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator*(const Vector<type,nComp> &a,const Vector<type,nComp> &b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a[i]*b[i];
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator/(const Vector<type,nComp> &a,const Vector<type,nComp> &b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a[i]/b[i];
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator%(const Vector<type,nComp> &a,const Vector<type,nComp> &b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a[i]%b[i];
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator+(const Vector<type,nComp> &a,const type b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a[i]+b;
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator-(const Vector<type,nComp> &a,const type b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a[i]-b;
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator*(const Vector<type,nComp> &a,const type b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a[i]*b;
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator/(const Vector<type,nComp> &a,const type b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a[i]/b;
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator%(const Vector<type,nComp> &a,const type b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a[i]%b;
	return r;
}

// operators
template <typename type,int nComp>
Vector<type,nComp> operator+(const type a,const Vector<type,nComp> &b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a+b[i];
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator-(const type a,const Vector<type,nComp> &b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a-b[i];
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator-(const Vector<type,nComp> &a)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = -a[i];
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator*(const type a,const Vector<type,nComp> &b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a*b[i];
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator/(const type a,const Vector<type,nComp> &b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a/b[i];
	return r;
}

template <typename type,int nComp>
Vector<type,nComp> operator%(const type a,const Vector<type,nComp> &b)
{
	Vector<type,nComp> r;
	for(int i=0;i<nComp;i++) r[i] = a%b[i];
	return r;
}

template <typename type,int nComp>
type dot(const Vector<type,nComp> &a,const Vector<type,nComp> &b)
{
	type r = 0;
	for(int i=0;i<nComp;i++) r += a[i]*b[i];
	return r;
}

template <typename type,int nComp>
bool operator==(const Vector<type,nComp> &a,const Vector<type,nComp> &b)
{
	for(int i=0;i<nComp;i++) if(a[i]!=b[i]) return false;
	return true;
}

template <typename type,int nComp>
bool operator!=(const Vector<type,nComp> &a,const Vector<type,nComp>&b)
{
	for(int i=0;i<nComp;i++) if(a[i]!=b[i]) return true;
	return false;
}

template <typename type>
V3<type> operator ^(const V3<type>  &a,const V3<type>  &b)
{
	return V3<type> (
		a.y()*b.z()-a.z()*b.y(),
		a.z()*b.x()-a.x()*b.z(),
		a.x()*b.y()-a.y()*b.x());
};
