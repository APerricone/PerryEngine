#include "dynamicmesh.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "mymacros.h"
#include "log.h"

#include <stdio.h>
#include <string.h>
#include "opengl.h"

CMesh::CMesh()
{
	m_bNormals = false;
	m_bColors = false;
	m_bTexCoords = false;

	m_pVerticesArray = NULL;
	m_pIndicesVoid = NULL;
	
	m_nVertices = 0;
	m_nFloatPerVertex = 0;
	m_iNormalDelta = 0;
	m_iColorDelta = 0;
	m_iTexCoordDelta = 0;
	m_nIndices = 0;
	
	m_glVBuffer =0;
	m_glIBuffer = 0;
}

CMesh::~CMesh()
{
	SAFE_DELETE_ARRAY(m_pVerticesArray);
	SAFE_DELETE_ARRAY(m_pIndices16);
	SAFE_DELETE_GL_BUFFER(m_glVBuffer);
	SAFE_DELETE_GL_BUFFER(m_glIBuffer);
}

void CMesh::Init(int nVertices,int nIndices,bool normals,bool colors,bool textures)
{
	m_bNormals = normals;
	m_bColors = colors;
	m_bTexCoords =textures;
	m_nVertices = nVertices;
	m_nIndices = nIndices;

	int nIgnored = (m_nIndices%3);
	if( nIgnored != 0)
	{
		ILog::Error("CMesh::Init: wrong number of vertices: ignored %i vertices\n",nIgnored);
		m_nIndices -= nIgnored;
	}
	m_bUse16bit = m_nVertices < 0xFFF0;
	if(m_bUse16bit)
	{
		m_pIndices16 =new unsigned short[m_nIndices];
	} else
	{
		m_pIndices32 = new unsigned long[m_nIndices];
	}
	
	m_nFloatPerVertex = 3;
	if(m_bNormals) m_nFloatPerVertex += 3;
	if(m_bColors) m_nFloatPerVertex += 4;
	if(m_bTexCoords) m_nFloatPerVertex += 2;

	m_pVerticesArray = new float[ m_nVertices * m_nFloatPerVertex];
	int delta = 3;
	if(m_bNormals)
	{
		m_iNormalDelta = delta;
		delta+=3;
	}
	if(m_bColors)
	{
		m_iColorDelta = delta;
		delta+=4;
	}
	if(m_bTexCoords)
	{
		m_iTexCoordDelta = delta;
		delta+=2;
	}

	ILog::Message(" - Created mesh with %i vertices and %i triangles\n", m_nVertices, m_nIndices/3);
}

void CMesh::SetNormal(int idx,const float3& normal)
{
	if( m_bNormals )
	{
		for(int j=0;j<3;j++)
		{
			m_pVerticesArray[idx*m_nFloatPerVertex+m_iNormalDelta+j] = normal[j];
		}
	}
}

void CMesh::SetTexture(int idx,const float2& texture)
{
	if( m_bTexCoords )
	{
		for(int j=0;j<2;j++)
		{
			m_pVerticesArray[idx*m_nFloatPerVertex+m_iTexCoordDelta+j] = texture[j];
		}
	}
}

void CMesh::SetColor(int idx,const float4& color)
{
	if( m_bColors )
	{
		for(int j=0;j<4;j++)
		{
			m_pVerticesArray[idx*m_nFloatPerVertex+m_iColorDelta+j] = color[j];
		}
	}
}

void CMesh::SetPoint(int idx,const float3& point)
{
	for(int j=0;j<3;j++)
	{
		m_pVerticesArray[idx*m_nFloatPerVertex+j] = point[j];
	}
}

void CMesh::SetIndex(int idx,int vidx)
{
	if(m_bUse16bit)
	{
		m_pIndices16[idx] = vidx;
	} else
	{
		m_pIndices32[idx] = vidx;
	}
}

void CMesh::Draw() const
{
	PreDraw();
	ReDraw();
	EndDraw();
}

void CMesh::PreDraw() const
{
	float* base = 0;
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_glVBuffer);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_glIBuffer);
	if(m_bNormals)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT,sizeof(float)*m_nFloatPerVertex,&base[m_iNormalDelta]);
	} else
		glDisableClientState(GL_NORMAL_ARRAY);
	if(m_bTexCoords)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,sizeof(float)*m_nFloatPerVertex,&base[m_iTexCoordDelta]);
	} else
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(m_bColors)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_FLOAT,sizeof(float)*m_nFloatPerVertex,&base[m_iColorDelta]);
	} else
		glDisableClientState(GL_COLOR_ARRAY);


	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,sizeof(float)*m_nFloatPerVertex,base);
}

/*void CMesh::Draw(
		unsigned int idPosition,
		unsigned int idNormal,
		unsigned int idColor,
		unsigned int idTexture) const
{
	PreDraw(idPosition,idNormal,idColor,idTexture);
	ReDraw();
	EndDraw();
}

void CMesh::PreDraw(
		unsigned int idPosition,
		unsigned int idNormal,
		unsigned int idColor,
		unsigned int idTexture) const
{
	float* base = 0;
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vBuffer);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, iBuffer);
	if( idNormal != 0xFFFFFFFF)
	{
		if(m_bNormals)
		{
			glEnableVertexAttribArray(idNormal);
			glVertexAttribPointer(idNormal,3,GL_FLOAT,GL_FALSE,sizeof(float)*nFloatPerVertex,&base[normalDelta]);
		} else
		{
			glDisableVertexAttribArray(idNormal);

		}
	}
	if( idTexture != 0xFFFFFFFF)
	{
		if(m_bTexCoords)
		{
			glEnableVertexAttribArray(idTexture);
			glVertexAttribPointer(idTexture,2,GL_FLOAT,GL_FALSE,sizeof(float)*nFloatPerVertex,&base[texCoordsDelta]);
		} else
			glDisableVertexAttribArray(idTexture);
	}
	if( idColor != 0xFFFFFFFF)
	{
		if(m_bColors)
		{
			glEnableVertexAttribArray(idColor);
			glVertexAttribPointer(idColor,4,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(float)*nFloatPerVertex,&base[colorDelta]);
		} else
			glDisableVertexAttribArray(idColor);
	}

	glEnableVertexAttribArray(idPosition);
	glVertexAttribPointer(idPosition,3,GL_FLOAT,GL_FALSE,sizeof(float)*nFloatPerVertex,base);
}*/

void CMesh::ReDraw() const
{
	glDrawRangeElements(GL_TRIANGLES,0,m_nVertices,m_nIndices,m_bUse16bit? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,0);
}

void CMesh::EndDraw()
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

float CMesh::LineCollision(const float3& start,const float3& dir,float len)
{
	float currMin = FLT_MAX;
	std::vector<CCollisionTriangle>::iterator i;
	for(i=m_aoCollisionTriangles.begin();i!=m_aoCollisionTriangles.end();++i)
	{
		float thisColl = (*i).LineCollision(start,dir,len);
		if( thisColl < FLT_MAX )
			currMin = thisColl;
	}
	return currMin;
}

void CMesh::CalculateBoundings()
{	
	m_aoCollisionTriangles.reserve(m_nIndices/3);
	for(unsigned int t=0;t<m_nIndices;t+=3)
	{
		if(m_bUse16bit)
		{
			m_aoCollisionTriangles.push_back(
				CCollisionTriangle(	
					float3(&m_pVerticesArray[m_pIndices16[ t ] * m_nFloatPerVertex]),
					float3(&m_pVerticesArray[m_pIndices16[t+1] * m_nFloatPerVertex]),
					float3(&m_pVerticesArray[m_pIndices16[t+2] * m_nFloatPerVertex]) ));
		}
		else
		{
			m_aoCollisionTriangles.push_back(
				CCollisionTriangle(	
					float3(&m_pVerticesArray[m_pIndices32[ t ] * m_nFloatPerVertex]),
					float3(&m_pVerticesArray[m_pIndices32[t+1] * m_nFloatPerVertex]),
					float3(&m_pVerticesArray[m_pIndices32[t+2] * m_nFloatPerVertex]) ));
		}
	}

	//float3 m_f3MinPoint;
	//float3 m_f3MaxPoint;
	m_f3MinPoint.Set(FLT_MAX);
	m_f3MaxPoint.Set(-FLT_MAX);
	float *pPos = m_pVerticesArray;
	for(unsigned int i=0;i<m_nVertices;i++,pPos+=m_nFloatPerVertex)
	{
		if( pPos[0] < m_f3MinPoint[0] ) m_f3MinPoint[0] = pPos[0];
		if( pPos[1] < m_f3MinPoint[1] ) m_f3MinPoint[1] = pPos[1];
		if( pPos[2] < m_f3MinPoint[2] ) m_f3MinPoint[2] = pPos[2];
		if( pPos[0] > m_f3MaxPoint[0] ) m_f3MaxPoint[0] = pPos[0];
		if( pPos[1] > m_f3MaxPoint[1] ) m_f3MaxPoint[1] = pPos[1];
		if( pPos[2] > m_f3MaxPoint[2] ) m_f3MaxPoint[2] = pPos[2];
	}
	m_f4BoundingSphere.Set( 
		( m_f3MinPoint + m_f3MaxPoint ) * 0.5f, 
		( m_f3MaxPoint - m_f3MinPoint ).GetLen() * 0.5f);
}

#include "miniball.h"
void CMesh::CalculateMinimalSphere()
{
	if( m_pVerticesArray == NULL )
	{
		ILog::Error("CMesh::CalculateMinimalSphere called without cpu copy\n");
		return;
	}
	Miniball<float,3> tmp;
	float *pPos = m_pVerticesArray;
	for(unsigned int i=0;i<m_nVertices;i++,pPos+=m_nFloatPerVertex)
	{
		tmp.check_in(float3(pPos));
	}
	tmp.build();
	m_f4BoundingSphere.Set( tmp.center(), sqrtf(tmp.squared_radius()) );
}

bool CMesh::UpdateBuffers()
{
	if( m_pVerticesArray == NULL || m_pIndicesVoid==NULL )
	{
		ILog::Error("CMesh::Init called without cpu copy\n");
		return false;
	}
	if(m_glVBuffer == 0) glGenBuffers(1, &m_glVBuffer);
	if(m_glIBuffer == 0) glGenBuffers(1, &m_glIBuffer);

	glBindBuffer(GL_ARRAY_BUFFER_ARB, m_glVBuffer);
	glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(float)*m_nFloatPerVertex*m_nVertices,m_pVerticesArray, GL_STATIC_DRAW_ARB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, m_glIBuffer);
	if(m_bUse16bit)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(unsigned short)*m_nIndices, m_pIndices16, GL_STATIC_DRAW_ARB);
	else
		glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(unsigned long)*m_nIndices, m_pIndices32, GL_STATIC_DRAW_ARB);

	glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	return true;
}

#define header "PerryMeshRaw"
void Testings()
{
	COMPILE_TIME_ASSERT(sizeof(unsigned char) == 1);
	COMPILE_TIME_ASSERT(sizeof(long long) == 8);
	COMPILE_TIME_ASSERT(sizeof(unsigned short) == 2);
	COMPILE_TIME_ASSERT(sizeof(unsigned long) == 4);
}

bool CMesh::LoadRaw(const char* i_strFilename)
{
	if( m_pVerticesArray != NULL || m_pIndicesVoid!=NULL )
	{
		RemoveCPUCopy();
		ILog::Message("WARNING: CMesh::LoadRaw old data deleted\n");
	}
	ILog::Message("Loading %s...\n",i_strFilename);
	FILE *f = fopen(i_strFilename,"rb");
	if( f == 0)
	{
		ILog::Error("unable to open %s for reading\n", i_strFilename);
		return false ;
	}
	char *hh = new char[strlen(header)];
#define MY_READ(dest,size) if( fread(dest,1,size,f) != size ) \
		{ \
			ILog::Error("reading error in %s\n", i_strFilename); \
			fclose(f); \
			return false ; \
		}
	MY_READ(hh,strlen(header));
	if( memcmp(hh,header,strlen(header)) != 0)
	{
		delete [] hh;
		ILog::Error("invalid header in %s\n", i_strFilename);
		fclose(f);
		return false ;
	}
	delete [] hh;
	long long valToRead; //read 64bit data
	MY_READ(&valToRead,8);  m_nFloatPerVertex = (unsigned int)(valToRead);
	MY_READ(&valToRead,8);  m_nVertices = (unsigned int)(valToRead);
	MY_READ(&valToRead,8);  m_nIndices = (unsigned int)(valToRead);
	MY_READ(&valToRead,8);  m_iNormalDelta = (unsigned int)(valToRead);
	MY_READ(&valToRead,8);  m_iColorDelta = (unsigned int)(valToRead);
	MY_READ(&valToRead,8);  m_iTexCoordDelta = (unsigned int)(valToRead);
	unsigned char bToRead;
	MY_READ(&bToRead,1);  m_bUse16bit = bToRead!=0;
	MY_READ(&bToRead,1);  m_bNormals = bToRead!=0;
	MY_READ(&bToRead,1);  m_bColors = bToRead!=0;
	MY_READ(&bToRead,1);  m_bTexCoords = bToRead!=0;
	m_pVerticesArray = new float[m_nFloatPerVertex*m_nVertices];
	MY_READ(m_pVerticesArray,sizeof(float)*m_nFloatPerVertex*m_nVertices);
	if( m_bUse16bit )
	{
		m_pIndices16 = new unsigned short[m_nIndices];
		MY_READ(m_pIndices16,sizeof(unsigned short)*m_nIndices);
	}
	else
	{
		m_pIndices32 = new unsigned long[m_nIndices];
		MY_READ(m_pIndices32,sizeof(unsigned long)*m_nIndices);
	}
	fclose(f);
	ILog::Message("Done. Loaded mesh with %i vertices and %i triangles\n", m_nVertices, m_nIndices/3);
	CalculateBoundings();
	UpdateBuffers();
	return true;
}

bool CMesh::SaveRaw(const char* i_strFilename)
{
	if( m_pVerticesArray == NULL || m_pIndicesVoid==NULL )
	{
		ILog::Error("CMesh::SaveRaw called without cpu copy\n");
		return false ;
	}
	printf("Saving %s...",i_strFilename);
	FILE *f = fopen(i_strFilename,"wb");
	if( f == 0)
	{
		ILog::Error("unable to open %s for writing\n", i_strFilename);
		return false ;
	}
	fwrite(header,1,strlen(header),f);
	long long valToWrite; //write 64bit data
	valToWrite = m_nFloatPerVertex;		fwrite(&valToWrite,8,1,f);
	valToWrite = m_nVertices;			fwrite(&valToWrite,8,1,f);
	valToWrite = m_nIndices;			fwrite(&valToWrite,8,1,f);
	valToWrite = m_iNormalDelta;		fwrite(&valToWrite,8,1,f);
	valToWrite = m_iColorDelta;			fwrite(&valToWrite,8,1,f);
	valToWrite = m_iTexCoordDelta;		fwrite(&valToWrite,8,1,f);
	unsigned char bToWrite;
	bToWrite = m_bUse16bit ? 1 : 0;		fwrite(&bToWrite,1,1,f);
	bToWrite = m_bNormals ? 1 : 0;		fwrite(&bToWrite,1,1,f);
	bToWrite = m_bColors ? 1 : 0;		fwrite(&bToWrite,1,1,f);
	bToWrite = m_bTexCoords ? 1 : 0;	fwrite(&bToWrite,1,1,f);
	fwrite(m_pVerticesArray,sizeof(float)*m_nFloatPerVertex,m_nVertices,f);
	if( m_bUse16bit )
		fwrite(m_pIndices16,sizeof(unsigned short),m_nIndices,f);
	else
		fwrite(m_pIndices32,sizeof(unsigned long),m_nIndices,f);
	fclose(f);
	ILog::Message("Done\n");
	return true;
}

void CMesh::RemoveCPUCopy()
{
	SAFE_DELETE_ARRAY(m_pVerticesArray);
	if(m_bUse16bit)
	{
		SAFE_DELETE_ARRAY(m_pIndices16);
	} else
	{
		SAFE_DELETE_ARRAY(m_pIndices32);
	}
}
