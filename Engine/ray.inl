
template<typename type,int nComp>
type Ray<type,nComp>::PointDistance(const Point &a,type& time)
{
	// P = s + d*t
	// find P1 -> a.d = P1.d && P1 = s + d*t
	// a.d = (s+d*t).d = s.d + d.d*t
	// t = a.d - s.d / d.d
	time = dot((a-start),direx) / dot(direx,direx);
	Point P1 = start + direx * time;
	return (a-P1).GetLen();
}

template<typename type>
type Ray2<type>::RayDistance(const Ray<type,2> &b,type& timeOnThis,type& timeOnB)
{
	if( dot(direx,b.GetDirex()) == 0 )
	{// parallel
		timeOnThis = timeOnB = 0;
		// check if b.GetStart() = start + t * direx
		Point startDiff = start-b.GetStart();
		if( dot(startDiff, direx) )
		{//same
			return 0;
		} else
		{
			return startDiff.GetLen();
		}
	}
	// p = start + direx * timeOnThis
	// q = b.GetStart() + b.GetDirex() * timeOnB
	// start + direx * timeOnThis = b.GetStart() + b.GetDirex() * timeOnB
	// direx * timeOnThis - b.GetDirex() * timeOnB = -(start - b.GetStart())
	// l = -(s-a).x --- m = direx.x --- n =-b.GetDirex().x
	// o = -(s-a).y --- p = direx.y --- q =-b.GetDirex().y
	Point startDiff = start-b.GetStart();
	type l =-startDiff.x;
	type m = direx.x;
	type n =-b.GetDirex().x;
	type o =-startDiff.y;
	type p = direx.y;
	type q =-b.GetDirex().y;
	// m*t + n*r = l
	// p*t + q*r = o
	type det = m * q - p * n; // it is the same of dor( direx, b.GetDirex()) so it is not 0
	type dett = l * q - o * n;
	type detr = m * o - p * l;
	timeOnThis = dett / det;
	timeOnB = detr / det;
	return 0;
}

template<typename type>
type Ray3<type>::RayDistance(const Ray<type,3> &b,type& timeOnThis,type& timeOnB)
{
	// let's call
	// start -> s
	// direx -> d
	// b.GetStart() -> a
	// b.GetDirex() -> b
	// timeOnThis -> t
	// timeOnB -> r
	// see:
	// P = s + d*t
	// Q = a + b*r
	// find P1 and Q1 -> (P1-Q1).d == 0 && (P1-Q1).b == 0
	// P1-Q1 = s + d*t - (a + b*r) = (s-a) + d*t - b*r
	// (P1-Q1).d = (s-a).d + d.d*t - b.d*r
	// (P1-Q1).b = (s-a).b + d.b*t - b.b*r
	// l = -(s-a).d --- m = d.d --- n =-b.d
	// o = -(s-a).b --- p = d.b --- q =-b.b
	Point startDiff = start-b.GetStart();
	type l =-dot(startDiff,direx);
	type m = dot(direx,direx);
	type n =-dot(b.GetDirex(),direx);
	type o =-dot(startDiff,b.GetDirex());
	type p = dot(direx,b.GetDirex());
	type q =-dot(b.GetDirex(),b.GetDirex());
	// m*t + n*r = l
	// p*t + q*r = o
	type det = m * q - p * n;
	type dett = l * q - o * n;
	type detr = m * o - p * l;
	if( det == 0 )
	{// parallel
		timeOnThis = timeOnB = 0;
		if( dett == 0 )
			return 0; // same
		else
			return startDiff.GetLen(); // distanced
	}
	timeOnThis = dett / det;
	timeOnB = detr / det;
	Point p1 = start + direx * timeOnThis;
	Point q1 = b.GetStart() + b.GetDirex() * timeOnB;
	return (p1-q1).GetLen();
}

template<typename type>
bool Ray3<type>::PlaneIntersection(const Point &p, const Point &n, type& timeOnThis)
{
	// let's call
	// start -> s
	// direx -> d
	// timeOnThis -> t
	// P1 = s + d*t
	// P1(dot)n = p(dot)n
	// s(dot)n + d(dot)n*t = p(dot)n
	// t = (p(dot)n - s(dot)n) / (d(dot)n)
	type den = dot(direx,n);
	if( den == 0 )
	{
		return false; // parallel
	}
	timeOnThis = dot((p - start),n) / den;
	return true;
}

template<typename type,int nComp>
type Ray<type,nComp>::SegmentDistance(const Ray<type,nComp> &b,type& timeOnThis,type& timeOnB)
{
	type val = RayDistance(b,timeOnThis,timeOnB);
	if( timeOnB>=0 && timeOnB<=1 ) return val;
	if( timeOnB<0 )
	{
		timeOnB = 0;
		return PointDistance(b.GetStart(),timeOnThis);
	}
	timeOnB = 1;
	return PointDistance(b.GetStart()+b.GetDirex(),timeOnThis);
}
