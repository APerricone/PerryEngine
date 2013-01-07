#pragma once

#include "vector.h"
#include <limits>

#ifdef max
#undef max
#endif

template<typename type,int nComp>
class Sphere : public Vector<type,nComp>
{
protected:
	using Vector<type,nComp>::v;
	typedef Vector<type,nComp> tPoint;
	type r;
public:
	Sphere() : tPoint(), r(0) {}
	Sphere(const tPoint& _c,type _r) : tPoint(_c), r(_r) {}

	void Reset() { tPoint::Set((type)0); r=0; }
	void Set(const tPoint& _c,type _r) { tPoint::operator =(_c); r=_r; }
	void SetCenter(const tPoint& _v) {  tPoint::operator =(_v); }
	void SetRadius(const type _r) { r = _r; }

	const tPoint GetCenter() const { return *this; }
	type GetRadius() const { return r; }

	Sphere<type,nComp>& operator+=(const Sphere<type,nComp>& b)
	{
		tPoint delta = GetCenter() - b.GetCenter();
		float dist = delta.Normalize();
		if( (dist+b.GetRadius()) < GetRadius() )
			return *this;
		if( (dist+GetRadius()) < b.GetRadius() )
		{
			*this = b;
			return *this;
		}
		SetRadius( ( dist + GetRadius() + b.GetRadius() ) / 2 );
		delta *= GetRadius() - b.GetRadius();
		SetCenter( b.GetCenter() + delta );
		return *this;
	}

	type LineCollision(const tPoint& p,const tPoint& d,type len = std::numeric_limits<type>::max())
	{
		// distPoint
		// |------|
		// |  radiusCollision
		// |      |----|* radius
		// p -->d------/
		//  \_    |   /
		//    \_  | </-- distFromLine
		//    ^ \_| /
		//    |   c
		//distFromStart
		//
		// see sphere1.svg
		type r2 = GetRadius()*GetRadius();
		type distFromStart2 = (GetCenter()-p).GetLenSqr();
		if( distFromStart2 < r2 )
			return 0;
		type distPoint = dot((GetCenter()-p),d); //NOTE: d is normalized!
		type distFromLine2 = (distFromStart2-distPoint*distPoint);
		if( distFromLine2 > r2 )
			return std::numeric_limits<type>::max();
		type radiusCollision = (type)(sqrt(r2 - distFromLine2));
		float t = distPoint - radiusCollision;
		if( t>0 && t<len )
			return t;
		return std::numeric_limits<type>::max();
	}
};

typedef Sphere<float,2> floatCircle;
typedef Sphere<double,2> doubleCircle;
typedef Sphere<float,3> floatSphere;
typedef Sphere<double,3> doubleSphere;

