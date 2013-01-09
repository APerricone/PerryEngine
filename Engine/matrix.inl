
template<typename type,int nComp>
inline void SquareMatrix<type,nComp>::Zero()
{
	for(int i=0;i<nComp*nComp;i++) m[i]=0;
}

template<typename type,int nComp>
inline void SquareMatrix<type,nComp>::Identity()
{
	for(int i=0,j=0;j<nComp;j++)
		for(int k=0;k<nComp;k++,i++)
		{
			m[i] = (j==k)? (type)1 : (type)0;
		}
}

template<typename type,int nComp>
inline void SquareMatrix<type,nComp>::GetTranspose(SquareMatrix<type,nComp>& oDest) const
{
	for (int i = 0; i < nComp; i++)
		for (int j = 0; j < nComp; j++)
		{
			oDest[i+j*nComp] = m[i*nComp+j];
		}
}

template<typename type,int nComp>
inline void SquareMatrix<type,nComp>::GetRotatePart(SquareMatrix<type,nComp-1>& oDest) const
{
	int i0=0,i1=0;
	for(int j=0;j<nComp-1;j++)
	{
		for(int k=0;k<nComp-1;k++)
		{
			oDest[i1] = m[i0];
			i0++;
			i1++;
		}
		i0++;
	}

}

template<typename type,int nComp>
inline void SquareMatrix<type,nComp>::Multiply(const SquareMatrix<type,nComp>& a,const SquareMatrix<type,nComp>& b)
{
	int i=0;
	for(int j=0;j<nComp;j++)
		for(int k=0;k<nComp;k++)
		{
			m[i] = 0;
			for(int l=0;l<nComp;l++)
			{
				m[i] += a[l+j*nComp] * b[l*nComp+k];
			}
			i++;
		}
}
template<typename type,int nComp>
inline Vector<type,nComp> SquareMatrix<type,nComp>::Transform(const Vector<type,nComp>& iVec) const
{
	Vector<type,nComp> r((type)0);
	for(int i=0;i<nComp;i++)
		for(int j=0;j<nComp;j++)
		{
			r[j] += m[i*nComp+j] * iVec[i];
		}
	return r;

}

template<typename type,int nComp>
inline Sphere<type,nComp-1> SquareMatrix<type,nComp>::TransformSphere(const Sphere<type,nComp-1>& iVec) const
{
	Vector<type,nComp> center = Transform(Vector<type,nComp>(iVec.GetCenter(),1.f));
	type scale = GetRow(0).GetLenSqr();
	for(int i=1;i<nComp-1;i++)
	{
		type s = GetRow(1).GetLenSqr();
		if( s > scale) scale = s;
	}

	float radius = iVec.GetRadius() * sqrtf(scale);
	return Sphere<type,nComp-1>(Vector<type,nComp-1>(center),radius);
}

template<typename type>
inline bool Matrix2<type>::GetInvert(SquareMatrix<type,2>& inv) const
{
	inv[0] = m[3];
	inv[1] =-m[1];
	inv[2] =-m[2];
	inv[3] = m[0];

	type det = m[0] * inv[0] + m[1] * inv[2];
	if (det == 0)
		return false;
	for (int i = 0; i < 4; i++)
		inv[i] /= det;
	return true;
}

template<typename type>
inline bool Matrix3<type>::GetInvert(SquareMatrix<type,3>& inv) const
{
	inv[0] = m[4]*m[8]-m[5]*m[7];
	inv[1] =-m[1]*m[8]+m[2]*m[7];
	inv[2] = m[1]*m[5]-m[2]*m[4];

	inv[3] =-m[3]*m[8]+m[5]*m[6];
	inv[4] = m[0]*m[8]-m[2]*m[6];
	inv[5] =-m[0]*m[5]+m[2]*m[3];

	inv[6] = m[3]*m[7]-m[4]*m[6];
	inv[7] =-m[0]*m[7]+m[1]*m[6];
	inv[8] = m[0]*m[4]-m[1]*m[3];

	type det = m[0] * inv[0] + m[1] * inv[1] + m[2] * inv[2] ;
	if (det == 0)
		return false;
	for (int i = 0; i < 9; i++)
		inv[i] /= det;
	return true;
}

template<typename type>
inline bool Matrix4<type>::GetInvert(SquareMatrix<type,4>& inv) const
{
	inv[ 0] =  m[ 5]*m[10]*m[15] - m[ 5]*m[11]*m[14] - m[ 9]*m[ 6]*m[15]
			 + m[ 9]*m[ 7]*m[14] + m[13]*m[ 6]*m[11] - m[13]*m[ 7]*m[10];
	inv[ 4] = -m[ 4]*m[10]*m[15] + m[ 4]*m[11]*m[14] + m[ 8]*m[ 6]*m[15]
			 - m[ 8]*m[ 7]*m[14] - m[12]*m[ 6]*m[11] + m[12]*m[ 7]*m[10];
	inv[ 8] =  m[ 4]*m[ 9]*m[15] - m[ 4]*m[11]*m[13] - m[ 8]*m[ 5]*m[15]
			 + m[ 8]*m[ 7]*m[13] + m[12]*m[ 5]*m[11] - m[12]*m[ 7]*m[ 9];
	inv[12] = -m[ 4]*m[ 9]*m[14] + m[ 4]*m[10]*m[13] + m[ 8]*m[ 5]*m[14]
			 - m[ 8]*m[ 6]*m[13] - m[12]*m[ 5]*m[10] + m[12]*m[ 6]*m[ 9];
	inv[ 1] = -m[ 1]*m[10]*m[15] + m[ 1]*m[11]*m[14] + m[ 9]*m[ 2]*m[15]
			 - m[ 9]*m[ 3]*m[14] - m[13]*m[ 2]*m[11] + m[13]*m[ 3]*m[10];
	inv[ 5] =  m[ 0]*m[10]*m[15] - m[ 0]*m[11]*m[14] - m[ 8]*m[ 2]*m[15]
			 + m[ 8]*m[ 3]*m[14] + m[12]*m[ 2]*m[11] - m[12]*m[ 3]*m[10];
	inv[ 9] = -m[ 0]*m[ 9]*m[15] + m[ 0]*m[11]*m[13] + m[ 8]*m[ 1]*m[15]
			 - m[ 8]*m[ 3]*m[13] - m[12]*m[ 1]*m[11] + m[12]*m[ 3]*m[ 9];
	inv[13] =  m[ 0]*m[ 9]*m[14] - m[ 0]*m[10]*m[13] - m[ 8]*m[ 1]*m[14]
			 + m[ 8]*m[ 2]*m[13] + m[12]*m[ 1]*m[10] - m[12]*m[ 2]*m[ 9];
	inv[ 2] =  m[ 1]*m[ 6]*m[15] - m[ 1]*m[ 7]*m[14] - m[ 5]*m[ 2]*m[15]
			 + m[ 5]*m[ 3]*m[14] + m[13]*m[ 2]*m[ 7] - m[13]*m[ 3]*m[ 6];
	inv[ 6] = -m[ 0]*m[ 6]*m[15] + m[ 0]*m[ 7]*m[14] + m[ 4]*m[ 2]*m[15]
			 - m[ 4]*m[ 3]*m[14] - m[12]*m[ 2]*m[ 7] + m[12]*m[ 3]*m[ 6];
	inv[10] =  m[ 0]*m[ 5]*m[15] - m[ 0]*m[ 7]*m[13] - m[ 4]*m[ 1]*m[15]
			 + m[ 4]*m[ 3]*m[13] + m[12]*m[ 1]*m[ 7] - m[12]*m[ 3]*m[ 5];
	inv[14] = -m[ 0]*m[ 5]*m[14] + m[ 0]*m[ 6]*m[13] + m[ 4]*m[ 1]*m[14]
			 - m[ 4]*m[ 2]*m[13] - m[12]*m[ 1]*m[ 6] + m[12]*m[ 2]*m[ 5];
	inv[ 3] = -m[ 1]*m[ 6]*m[11] + m[ 1]*m[ 7]*m[10] + m[ 5]*m[ 2]*m[11]
			 - m[ 5]*m[ 3]*m[10] - m[ 9]*m[ 2]*m[ 7] + m[ 9]*m[ 3]*m[ 6];
	inv[ 7] =  m[ 0]*m[ 6]*m[11] - m[ 0]*m[ 7]*m[10] - m[ 4]*m[ 2]*m[11]
			 + m[ 4]*m[ 3]*m[10] + m[ 8]*m[ 2]*m[ 7] - m[ 8]*m[ 3]*m[ 6];
	inv[11] = -m[ 0]*m[ 5]*m[11] + m[ 0]*m[ 7]*m[ 9] + m[ 4]*m[ 1]*m[11]
			 - m[ 4]*m[ 3]*m[ 9] - m[ 8]*m[ 1]*m[ 7] + m[ 8]*m[ 3]*m[ 5];
	inv[15] =  m[ 0]*m[ 5]*m[10] - m[ 0]*m[ 6]*m[ 9] - m[ 4]*m[ 1]*m[10]
			 + m[ 4]*m[ 2]*m[ 9] + m[ 8]*m[ 1]*m[ 6] - m[ 8]*m[ 2]*m[ 5];

	float det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
	if (det == 0)
		return false;
	for (int i = 0; i < 16; i++)
		inv[i] /= det;
	return true;
}

template<typename type,int nComp>
SquareMatrix<type,nComp> operator+(const SquareMatrix<type,nComp> &a,const SquareMatrix<type,nComp> &b)
{
	SquareMatrix<type,nComp> r;
	for(int i=0;i<nComp*nComp;i++)
		r[i] = a[i] + b[i];
	return r;
}

template<typename type>
void Matrix2<type>::Rotate(type cosa,type sina)
{
	m[0] = cosa;
	m[1] =-sina;
	m[2] = sina;
	m[3] = cosa;
}

template<typename type,int nComp>
void RotateX(float cosa,float sina,type* pDest)
{
	pDest[1+nComp*1] = cosa;
	pDest[2+nComp*1] =-sina;
	pDest[1+nComp*2] = sina;
	pDest[2+nComp*2] = cosa;
}

template<typename type,int nComp>
void RotateY(float cosa,float sina,type* pDest)
{
	pDest[0+nComp*0] = cosa;
	pDest[2+nComp*0] = sina;
	pDest[0+nComp*2] =-sina;
	pDest[2+nComp*2] = cosa;
}

template<typename type,int nComp>
void RotateZ(float cosa,float sina,type* pDest)
{
	pDest[0+nComp*0] = cosa;
	pDest[1+nComp*0] =-sina;
	pDest[0+nComp*1] = sina;
	pDest[1+nComp*1] = cosa;
}

template<typename type,int nCol>
void RotateAxis(const Vector<type,3>& axis,float c,float s,type* pDest)
{
	float i=1-c;

	//http://mathworld.wolfram.com/RodriguesRotationFormula.html
	pDest[0+nCol*0]=(axis[0]*axis[0])*i+		c;
	pDest[0+nCol*1]=(axis[0]*axis[1])*i-axis[2]*s;
	pDest[0+nCol*2]=(axis[0]*axis[2])*i+axis[1]*s;

	pDest[1+nCol*0]=(axis[1]*axis[0])*i+axis[2]*s;
	pDest[1+nCol*1]=(axis[1]*axis[1])*i+		c;
	pDest[1+nCol*2]=(axis[1]*axis[2])*i-axis[0]*s;

	pDest[2+nCol*0]=(axis[2]*axis[0])*i-axis[1]*s;
	pDest[2+nCol*1]=(axis[2]*axis[1])*i+axis[0]*s;
	pDest[2+nCol*2]=(axis[2]*axis[2])*i+		c;
}

template<typename type>
void Matrix3<type>::RotateX(float cosa,float sina)
{
	SquareMatrix<type,3>::Identity();
	::RotateX<type,3>(cosa,sina,m);
}

template<typename type>
void Matrix3<type>::RotateY(float cosa,float sina)
{
	SquareMatrix<type,3>::Identity();
	::RotateY<type,3>(cosa,sina,m);
}

template<typename type>
void Matrix3<type>::RotateZ(float cosa,float sina)
{
	SquareMatrix<type,3>::Identity();
	::RotateZ<type,3>(cosa,sina,m);
}

template<typename type>
void Matrix3<type>::RotateAxis(const Vector<type,3>& axis,float cosa,float sina)
{
	SquareMatrix<type,3>::Identity();
	::RotateAxis<type,3>(axis,cosa,sina,m);
}

template<typename type>
void Matrix4<type>::RotateX(float cosa,float sina)
{
	SquareMatrix<type,4>::Identity();
	::RotateX<type,3>(cosa,sina,m);
}

template<typename type>
void Matrix4<type>::RotateY(float cosa,float sina)
{
	SquareMatrix<type,4>::Identity();
	::RotateY<type,4>(cosa,sina,m);
}

template<typename type>
void Matrix4<type>::RotateZ(float cosa,float sina)
{
	SquareMatrix<type,4>::Identity();
	::RotateZ<type,4>(cosa,sina,m);
}

template<typename type>
void Matrix4<type>::RotateAxis(const Vector<type,3>& axis,float cosa,float sina)
{
	SquareMatrix<type,4>::Identity();
	::RotateAxis<type,4>(axis,cosa,sina,m);
}

template<typename type>
void Matrix3<type>::SetLookAt(const Vector<type,2>& p,const Vector<type,2>& l)
{
	Vector<type,2> d = (l-p).GetNormalized();
	GetPos(p);
	SetRg(d);
	SetUp(Vector<type,2>(d[0],-d[1]));
}

template<typename type>
void Matrix3<type>::Translate(const Point& t)
{
	SetPos( GetPos() + t );
}

template<typename type>
void Matrix4<type>::SetLookAt(const Vector<type,3>& p,const Vector<type,3>& l)
{
	Vector<type,3> d = (l-p).GetNormalized();
	Vector<type,3> a = (d^Vector<type,3>(0,1,0)).GetNormalized();
	Vector<type,3> u = (a^d).GetNormalized();
	SetPos(p);
	SetRg(d);
	SetAt(a);
	SetUp(u);
}

template<typename type>
void Matrix4<type>::Translate(const Point& t)
{
	SetPos( GetPos() + t );
}
