#pragma once

#include "vector.h"
#include <vector>

class CDynamicMesh;
class CBestFitCube;

class CMaterial
{
	CMaterial();
	~CMaterial();
	static CMaterial instance;
public:
	static bool Compile() { return instance.CompileInternal(); }

	static void SetIt() { instance.SetItInternal(); }
	static void Reset() { instance.ResetInternal(); }

	static void Update() { instance.UpdateInternal(); }


	struct Data
	{
		float4 f4Color;
		float4 f4Specular;
		unsigned int glDiffuseTex;

		Data() : 
			f4Color(1), 
			f4Specular(0,0,0,1), 
			glDiffuseTex(0) {}
		void Destroy();
	};
	static void SetData(const Data& i_data) { instance.SetDataInternal(i_data); }

private:
	CBestFitCube *m_pCubeTex;
	// openGL stuff
	unsigned int m_glVertex;
	unsigned int m_glFragment;
	unsigned int m_glProgram;
	unsigned int m_glWhiteTex;

	bool CompileInternal();
	void SetItInternal();
	void ResetInternal();
	void UpdateInternal();
	void SetDataInternal(const Data& i_data);
};
