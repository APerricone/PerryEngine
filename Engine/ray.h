#pragma once

#include "vector.h"

template<typename type,int nComp>
class Ray
{
public:
	typedef Vector<type,nComp> Point;
protected:
	Point start;
	Point direx;
public:
	Ray(const Point& s,const Point& d) : start(s), direx(d) {}

	type PointDistance(const Point &a,type& time);
	virtual type RayDistance(const Ray<type,nComp> &b,type& timeOnThis,type& timeOnB) = 0;
	type SegmentDistance(const Ray<type,nComp> &b,type& timeOnThis,type& timeOnB);

	const Point& GetStart() const { return start; }
	const Point& GetDirex() const { return direx; }
	Point GetPoint(const type& time) const { return start + direx * time; }
};

template<typename type>
class Ray2 : public Ray<type,2>
{
	using Ray<type,2>::start;
	using Ray<type,2>::direx;
	typedef typename Ray<type,2>::Point Point;
public:
	Ray2(const Point& s,const Point& d) : Ray<type,2>(s,d) {}
	virtual type RayDistance(const Ray<type,2> &b,type& timeOnThis,type& timeOnB);
};

template<typename type>
class Ray3 : public Ray<type,3>
{
	using Ray<type,3>::start;
	using Ray<type,3>::direx;
	typedef typename Ray<type,3>::Point Point;
public:
	Ray3(const Point& s,const Point& d) : Ray<type,3>(s,d) {}

	virtual type RayDistance(const Ray<type,3> &b,type& timeOnThis,type& timeOnB);
	bool PlaneIntersection(const Point &p, const Point &n, type& timeOnThis);
};

#include "ray.inl"

typedef Ray2<float> Ray2f;
typedef Ray3<float> Ray3f;
typedef Ray2<double> Ray2d;
typedef Ray3<double> Ray3d;
