#pragma once

#include "vector.h"
#include <limits>

template<typename type,int nComp>
class Capsule
{
protected:
	typedef Vector<type,nComp> tPoint;
	typedef SquareMatrix<type,nComp+1> tMatrix;
	tPoint m_tA,m_tB;
	tPoint m_tD;
	type m_tRadius;
	type m_tLen;
	tMatrix m_tLocal;
	tMatrix m_tInvLocal;
	virtual void CalcMatrices() = 0;
	virtual type LineCollisionInternal(const tPoint& p,const tPoint& d,type len) = 0;
	void Update();
public:
	Capsule() : m_tA(), m_tB(), m_tD(), m_tRadius(0) {}
	Capsule(const tPoint& a,const tPoint& b,const type& r) : m_tA(a), m_tB(b),
		m_tRadius(r) { Update(); }

	void Set(const tPoint& a,const tPoint& b,const type& r)
	{ m_tA=a; m_tB=b; m_tRadius=r; Update(); }
	void SetA(const tPoint& a) { m_tA = a; Update(); }
	void SetB(const tPoint& b) { m_tB = b; Update(); }
	const tPoint& GetA() const { return m_tA; }
	const tPoint& GetB() const { return m_tB; }

	type LineCollision(const tPoint& p,const tPoint& d,type len = std::numeric_limits<type>::max())
	{
		return LineCollisionInternal(p,d,len);
	}
};

template<typename type,int nComp>
void Capsule<type,nComp>::Update()
{
	m_tD = m_tB - m_tA;
	m_tLen = m_tD.GetLen();
	CalcMatrices();
}

template<typename type>
class Capsule2 : public Capsule<type,2>
{
	using Capsule<type,2>::m_tA;
	using Capsule<type,2>::m_tB;
	using Capsule<type,2>::m_tD;
	using Capsule<type,2>::m_tLen;
	using Capsule<type,2>::m_tLocal;
	using Capsule<type,2>::m_tInvLocal;
	using Capsule<type,2>::m_tRadius;
	typedef Vector<type,2> tPoint;
protected:
	void CalcMatrices()
	{
		Matrix3<type> *tM3 = reinterpret_cast<Matrix3<type> >(&m_tLocal);
		Vector<type,3> rg(m_tD / m_tLen,0);
		tM3->SetRg(rg);
		tM3->SetUp(Vector<type,3>(-rg[1],rg[0],0));
		tM3->SetPos( Vector<type,3>(m_tA,1) );
		tM3->GetInvert(m_tInvLocal);
	}

	virtual type LineCollisionInternal(const tPoint& _p,const tPoint& _d,type len)
	{
		tPoint p = m_tInvLocal.Transform(_p);
		tPoint d = m_tInvLocal.Transform(_d);
		// now the capsule is long X, search nearest point with y = radius
		// py + t*dy = radius --> t = (radius-py) / dy
		// py + t*dy = -radius --> t = -(radius+py) / dy
		type t = (m_tRadius-p[1]) / d[1];
		type t2 = -(m_tRadius+p[1]) / d[1];
		if( t<0 && t2<0 ) return std::numeric_limits<type>::max();
		if( t>len && t2>len ) return std::numeric_limits<type>::max();
		if( t2<t ) t = t2;
		type collx = p[0] + d[0] * t;
		if( collx < 0 )
		{
			Sphere<type,2> sph(m_tA,m_tRadius);
			return sph.LineCollision( _p,_d,len );
		}
		if( collx > m_tLen )
		{
			Sphere<type,2> sph(m_tB,m_tRadius);
			return sph.LineCollision( _p,_d,len );
		}
		return t;
	}
};

template<typename type>
class Capsule3 : public Capsule<type,3>
{
	using Capsule<type,3>::m_tA;
	using Capsule<type,3>::m_tB;
	using Capsule<type,3>::m_tD;
	using Capsule<type,3>::m_tLen;
	using Capsule<type,3>::m_tRadius;
	using Capsule<type,3>::m_tLocal;
	using Capsule<type,3>::m_tInvLocal;
	typedef Vector<type,3> tPoint;
protected:
	void CalcMatrices()
	{
		Matrix4<type> *tM4 = reinterpret_cast<Matrix4<type> >(&m_tLocal);
		Vector<type,3> rg(m_tD / m_tLen);
		Vector<type,3> up(0);
		if(fabs(rg[1])>0.9) up[2] = 1; else up[1] = 1;
		Vector<type,3> at = rg^up; at.Normalize();
		up=at^rg; up.Normalize();
		tM4->SetRg( Vector<type,4>(rg,0) );
		tM4->SetUp( Vector<type,4>(up,0) );
		tM4->SetAt( Vector<type,4>(at,0) );
		tM4->SetPos( Vector<type,4>(m_tA,1) );
		tM4->GetInvert(m_tInvLocal);
	}
	virtual type LineCollisionInternal(const tPoint& _p,const tPoint& _d,type len)
	{
		tPoint p = m_tInvLocal.Transform(_p);
		tPoint d = m_tInvLocal.Transform(_d);
		// now the capsule is long X, search nearest point with y^2+z^2 = radius^2
		// (py+dy*t)^2 + (pz+dz*t)^2 = radius^2
		// t^2 (dy^2+dz^2) + t 2*(py*dz+pz*dy) + py^2+pz^2-radius^2 = 0
		// a = (dy^2+dz^2)
		// b = 2*(py*dz+pz*dy)
		// c = py^2+pz^2-radius^2
		// det = b^2-4*a*c
		type a = d[1]*d[1]+d[2]*d[2];
		type b = 2* p[1]*d[2]+p[2]*d[1];
		type c = p[1]*p[1]+p[2]*p[2] - m_tRadius*m_tRadius;
		type det = b*b-4*a*c;
		if( det< 0 ) return std::numeric_limits<type>::max();
		det = type(sqrt(det));
		type t = (-b-det)/(2*a);
		type t2 = (-b+det)/(2*a);
		if( t<0 && t2<0 ) return std::numeric_limits<type>::max();
		if( t>len && t2>len ) return std::numeric_limits<type>::max();
		if( t2<t ) { t = t2; }
		type collx = p[0] + d[0] * t;
		if( collx < 0 )
		{
			Sphere<type,2> sph(m_tA,m_tRadius);
			return sph.LineCollision( _p,_d,len );
		}
		if( collx > m_tLen )
		{
			Sphere<type,2> sph(m_tB,m_tRadius);
			return sph.LineCollision( _p,_d,len );
		}
		return t;

	}
};


typedef Capsule2<float> Capsule2f;
typedef Capsule3<float> Capsule3f;
typedef Capsule2<double> Capsule2d;
typedef Capsule3<double> Capsule3d;
