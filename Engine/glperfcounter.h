#pragma once

#include "opengl.h"

class CGLPerfCounter
{
	CGLPerfCounter();
	~CGLPerfCounter();

	static CGLPerfCounter instance;
public:
	static void Deinit()					{ instance.InternalDeinit(); }

	static void Begin()						{ instance.InternalBegin(); }
	static void Mark(const char* i_strName)	{ instance.InternalMark(i_strName); }
	static void End()						{ instance.InternalEnd(); }

	static void SetupLegend(float i_fX,float i_fY,bool i_bTopAlign,bool i_bLeftAlign)
	{ instance.InternalSetupLegend(i_fX,i_fY,i_bTopAlign,i_bLeftAlign); }
	static void SetupGraph(float i_fX,float i_fY,float i_fW,float i_fH,bool i_bHoriz)
	{ instance.InternalSetupGraph(i_fX,i_fY,i_fW,i_fH,i_bHoriz); }
	static void PrintPreviousFrame(bool i_bOnlyMark)	
	{ instance.InternalPrintPreviousFrame(i_bOnlyMark); }

private:
	enum
	{
		c_nNameMaxLen = 20,
		c_nMaxMarkPerFrame = 20
	};

	unsigned int m_iCounter;
	char m_asNames[c_nMaxMarkPerFrame][c_nNameMaxLen+1];
	GLuint m_glQueries[c_nMaxMarkPerFrame];

	GLuint64 m_paResults[2][100];
	GLuint64 m_iTotal[2];
	unsigned int m_nRequest[2];
	unsigned int m_iCurrentId;

	void InternalDeinit();

	void InternalBegin();
	void InternalMark(const char* i_strName);
	void InternalEnd();

	struct LegendData
	{
		float iX;
		float iY;
		bool bTopAlign;
		bool bLeftAlign;
	} m_sLegend;
	struct Graph
	{
		float iX;
		float iY;
		float iWidth;
		float iHeight;
		bool bHorizontal;
	} m_sGraph;
	bool m_bGraph;

	void InternalSetupLegend(float i_fX,float i_fY,bool i_bTopAlign,bool i_bLeftAlign);
	void InternalSetupGraph(float i_fX,float i_fY,float i_fW,float i_fH,bool i_bHoriz);
	void InternalPrintPreviousFrame(bool i_bOnlyMark);
};
