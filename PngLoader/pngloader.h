#pragma once

#include "image.h"

class CPngLoader : public IImageLoader
{
	static CPngLoader loader;
public:
	static void Register();
	static void Unregister();
	
	virtual const char* GetName() { return "LibPng support"; }
	virtual bool IsSupported(const char *i_sExtension);
	virtual CImage* Load(const char *i_sPath);
	virtual bool Save(const char *i_sPath,const CImage* i_pImage);
};
