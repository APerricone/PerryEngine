#pragma once

#include "openclinterface.h"

class CHistogram
{
public:
	CHistogram();
	~CHistogram();

	void SetInput(unsigned int glTex);
	void Calculate();
	float* GetHistogram() const { return m_afHistrogram; }

private:
	cl_program m_clProgram;
	cl_kernel m_clKernel;
	cl_mem m_clInput;
	cl_mem m_clOutput;

	float m_afHistrogram[512];
};
