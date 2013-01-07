#include "glperfcounter.h"
#include "mymacros.h"
#include "font.h"
#include "vector.h"
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

CGLPerfCounter CGLPerfCounter::instance;

CGLPerfCounter::CGLPerfCounter()
{
	m_nRequest[0] = 0 ;
	m_nRequest[1] = 0 ;
	m_iCounter = 0;
	m_glQueries[0] = 0;
	m_iCurrentId =0;

	m_bGraph = false;
}

CGLPerfCounter::~CGLPerfCounter()
{
	if( m_glQueries[0] )
	{
		ILog::Error("*****************************************\n");
		ILog::Error("ERROR LEAK!!! call CGLPerfCounter::Deinit()\n");
		ILog::Error("*****************************************\n");
	}
}

void CGLPerfCounter::InternalDeinit()
{
	if( !GLEW_ARB_timer_query) return;

	if( m_glQueries[0] )
		glDeleteQueries(c_nMaxMarkPerFrame,m_glQueries);
	m_glQueries[0] = 0;
}

void CGLPerfCounter::InternalBegin()
{
	if( !GLEW_ARB_timer_query) return;
	m_iCounter =0;
	InternalMark("Begin");

	if( m_glQueries[0] == 0 )
	{
		glGenQueries(CGLPerfCounter::c_nMaxMarkPerFrame, m_glQueries);
	}
}

void CGLPerfCounter::InternalMark(const char* sName)
{
	if( !GLEW_ARB_timer_query) return;
	if(m_iCounter>=c_nMaxMarkPerFrame) return;

	int len = strlen(sName);

	if(len>c_nNameMaxLen) len=c_nNameMaxLen;
	memcpy(m_asNames[m_iCounter],sName,len);
	m_asNames[m_iCounter][len] = 0;
	glQueryCounter(m_glQueries[m_iCounter],GL_TIMESTAMP);
	m_iCounter++;

}

void CGLPerfCounter::InternalEnd()
{
	if( !GLEW_ARB_timer_query) return;
	if(m_glQueries)
	{
		if(m_iCounter<c_nMaxMarkPerFrame)
		{
			glQueryCounter(m_glQueries[m_iCounter],GL_TIMESTAMP);
			m_nRequest[m_iCurrentId] = m_iCounter;
		} else
			m_nRequest[m_iCurrentId] = m_iCounter-1;

		for(unsigned int i=0;i<m_nRequest[m_iCurrentId];i++)
		{
            glGetQueryObjectui64v(m_glQueries[i], GL_QUERY_RESULT, &m_paResults[m_iCurrentId][i]);
		}
		m_iTotal[m_iCurrentId] = m_paResults[m_iCurrentId][m_nRequest[m_iCurrentId]-1] - m_paResults[m_iCurrentId][0] ;

		m_iCurrentId = 1-m_iCurrentId;
	}
}

void CGLPerfCounter::InternalSetupLegend(float x,float y,bool topAlign,bool leftAlign)
{
	m_sLegend.iX = x;
	m_sLegend.iY = y;
	m_sLegend.bTopAlign = topAlign;
	m_sLegend.bLeftAlign = leftAlign;
}

void CGLPerfCounter::InternalSetupGraph(float x,float y,float w,float h,bool horiz)
{
	m_sGraph.iX = x;
	m_sGraph.iY = y;
	m_sGraph.iWidth = w;
	m_sGraph.iHeight = h;
	m_sGraph.bHorizontal = horiz;
	m_bGraph = true;
}

void CGLPerfCounter::InternalPrintPreviousFrame(bool onlyMark)
{
	if( !GLEW_ARB_timer_query) return;
	if( onlyMark ) 
	{
		InternalMark("Print PerfCounter");
		return;
	}

	// legend:
	float x = m_sLegend.iX;
	float y = m_sLegend.iY;
	if( !m_sLegend.bTopAlign ) y-= 15*(m_nRequest[1-m_iCurrentId]-1);
	if( !m_sLegend.bLeftAlign ) x-= 8*(c_nNameMaxLen+15);
	if( m_bGraph && m_sLegend.bLeftAlign) 
			x+=20;
	static const unsigned char colors[15][3] = {
		{255,0,0},
		{0,255,0},
		{0,0,255},
		{0,255,255},		
		{255,0,255},
		{255,255,0},
		{255,128,0},
		{255,0,128},
		{255,128,128},
		{128,255,0},
		{0,255,128},
		{128,255,128},
		{128,0,255},
		{0,128,255},
		{128,128,255}
	};
	float graphStart;
	if( m_sGraph.bHorizontal )
		graphStart = (float)m_sGraph.iX;
	else
		graphStart = (float)m_sGraph.iY;
	for(unsigned int i=1;i<m_nRequest[1-m_iCurrentId];i++)
	{
		float delta = (float)(m_paResults[1-m_iCurrentId][i]-m_paResults[1-m_iCurrentId][i-1]);
		float v = delta	/ (float)(m_iTotal[1-m_iCurrentId]);
		//glPrint(x,y,"%20s: %5.1f%%", m_asNames[i],v*100.f);
		float msec = floorf(delta / 1e6f);
		float usec = delta / 1e3f - msec*1000;
		if( msec!=0)
			glPrint(x,y,"%20s: %1.0f,%05.1f usec", m_asNames[i],msec,usec);
		else
			glPrint(x,y,"%20s:   %5.1f usec", m_asNames[i],usec);
		if( m_bGraph )
		{
			glColor3ubv(colors[(i-1)%15]);
			glBegin(GL_QUADS);
			glVertex2f(x-20.f,y-15.f);
			glVertex2f(x-20.f,y);
			glVertex2f(x-5.f,y);
			glVertex2f(x-5.f,y-15.f);

			float graphEnd;
			if( m_sGraph.bHorizontal )
			{
				graphStart = m_sGraph.iX + 
					(float)(m_paResults[1-m_iCurrentId][i-1] - m_paResults[1-m_iCurrentId][0]) * m_sGraph.iWidth / (float)(m_iTotal[1-m_iCurrentId]);
				graphEnd = m_sGraph.iX +
					(float)(m_paResults[1-m_iCurrentId][i] - m_paResults[1-m_iCurrentId][0]) * m_sGraph.iWidth / (float)(m_iTotal[1-m_iCurrentId]);

				glVertex2f(graphStart,m_sGraph.iY);
				glVertex2f(graphStart,m_sGraph.iY+m_sGraph.iHeight);
				glVertex2f(graphEnd,m_sGraph.iY+m_sGraph.iHeight);
				glVertex2f(graphEnd,m_sGraph.iY);
			} else
			{
				graphStart = m_sGraph.iY + 
					(float)(m_paResults[1-m_iCurrentId][i-1] - m_paResults[1-m_iCurrentId][0]) * m_sGraph.iHeight / (float)(m_iTotal[1-m_iCurrentId]);
				graphEnd = m_sGraph.iY + 
					(float)(m_paResults[1-m_iCurrentId][i] - m_paResults[1-m_iCurrentId][0]) * m_sGraph.iHeight / (float)(m_iTotal[1-m_iCurrentId]);
				glVertex2f(m_sGraph.iX,graphStart);
				glVertex2f(m_sGraph.iX,graphEnd);
				glVertex2f(m_sGraph.iX+m_sGraph.iWidth,graphEnd);
				glVertex2f(m_sGraph.iX+m_sGraph.iWidth,graphStart);
			}

			glEnd();
			glColor3f(1,1,1);
		}
		y+=15;
	}

	InternalMark("Print PerfCounter");
}
