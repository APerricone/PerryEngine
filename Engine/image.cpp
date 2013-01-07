#include "image.h"
#include "opengl.h"
#include "mymacros.h"

#include <math.h>
#include <string.h>

#include <algorithm>

#pragma region Strandard init

CImage::CImage()
{
	m_pBits = 0;
	m_iDimx = m_iDimy = m_iBpp = 0;
	m_bAlpha = false;
	m_pBits = 0;
}

CImage::~CImage()
{
	SAFE_DELETE_ARRAY(m_pBits);
}

void CImage::Create(unsigned int i_iDimx,unsigned int i_iDimy,unsigned int i_iBpp)
{
	if(m_pBits!=0)
	{
		ILog::Error("CImage::Create called on initialized image\n"); 
		return; 
	}
	m_iDimx = i_iDimx;
	m_iDimy = i_iDimy;
	m_iBpp = i_iBpp;
	m_iStride = m_iDimx*m_iBpp;
	if( m_iStride % 4 != 0 )
	{
		m_iStride += 4-m_iStride % 4;
	}
	m_pBits=new unsigned char[m_iStride*m_iDimy];
}

#pragma endregion

#pragma region OpenGL stuff
//*******************
//** OPEN GL STUFF **
//*******************
void CImage::GetFormat(unsigned int& format,unsigned int& intFormat)
{
	switch(m_iBpp)
	{
	case 1: 
		if( m_bAlpha )
		{
			format=GL_ALPHA;		intFormat=GL_ALPHA8;
		} else
		{
			format=GL_LUMINANCE;		intFormat=GL_LUMINANCE8;
		}
		break;
	case 2: format=GL_LUMINANCE_ALPHA;	intFormat=GL_LUMINANCE8_ALPHA8; break;
	case 3: format=GL_RGB;				intFormat=GL_RGB8; break;
	case 4: format=GL_RGBA;				intFormat=GL_RGBA8; break;
	}
}

unsigned int CImage::GetTexture2D()
{
	if(m_pBits==0)
	{
		ILog::Error("CImage::GetTexture2D called on uninitialized image\n"); 
		return 0; 
	}
	unsigned int glTexture;
	glGenTextures(1,&glTexture);
	glBindTexture(GL_TEXTURE_2D,glTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	GLenum format,intFormat;
	GetFormat(format,intFormat);
	glTexImage2D(GL_TEXTURE_2D,0,intFormat,m_iDimx,m_iDimy,0,format,GL_UNSIGNED_BYTE,m_pBits);	

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	glError();
	delete [] m_pBits; m_pBits=0;
	return glTexture;
}

unsigned int CImage::GetTexture3D(unsigned int i_iDepthAlongWidth,unsigned int i_iDepthAlongHeight)
{
	if(m_pBits==0)
	{
		ILog::Error("CImage::GetTexture3D called on uninitialized image\n"); 
		return 0; 
	}
	unsigned int glTexture;
	glGenTextures(1,&glTexture);
	glBindTexture(GL_TEXTURE_3D,glTexture);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	GLenum format,intFormat;
	GetFormat(format,intFormat);

	unsigned int dimx = m_iDimx / i_iDepthAlongWidth;
	unsigned int dimy = m_iDimy / i_iDepthAlongHeight;
	unsigned int dimz = i_iDepthAlongWidth * i_iDepthAlongHeight;
	unsigned int sliceDim = dimx * dimy;

	unsigned char* bits = new unsigned char[dimx*dimy*dimz*m_iBpp];
	unsigned char* pDest = bits;
	for(unsigned int z=0;z<dimz;z++) for(unsigned int y=0;y<dimy;y++)
	for(unsigned int x=0;x<dimx;x++) 
	{
		unsigned int sx = x + (z % i_iDepthAlongWidth) * dimx;
		unsigned int sy = y + (z / i_iDepthAlongWidth) * dimy;
		unsigned char* pSource = &(m_pBits[ ( sx + (sy * m_iDimx ) ) * m_iBpp]);
		for(unsigned int p=0;p<m_iBpp;p++)
		{
			*pDest = * pSource;
			pDest++;
			pSource++;
		}
	}
	glTexImage3D(GL_TEXTURE_3D,0,intFormat,dimx,dimy,dimz,0,format,GL_UNSIGNED_BYTE,bits);	
	glBindTexture(GL_TEXTURE_3D,0);

	glError();
	delete [] bits;
	return glTexture;
}

CImage* CImage::CreateFromOpenGLParam(int width,int height,int intformat,int& format)
{
	bool alpha = false;
	int bpp = -1;
	switch(intformat)
	{
	case GL_ALPHA8: alpha=true; bpp = 1; format = GL_ALPHA; break;
	case GL_LUMINANCE8:			bpp = 1; format = GL_LUMINANCE; break;
	case GL_LUMINANCE8_ALPHA8:	bpp = 2; format = GL_LUMINANCE_ALPHA; break;
	case GL_RGB8:				bpp = 3; format = GL_RGB; break;
	case GL_RGBA8:				bpp = 4; format = GL_RGBA; break;
	}
	if( bpp == -1 ) return NULL;
	CImage *pImage = new CImage();
	pImage->Create(width,height,bpp);
	pImage->SetAlphaSet( alpha );
	return pImage;
}

CImage* CImage::CreateFromTexture2D(unsigned int i_glResource)
{
	glError();

	glBindTexture(GL_TEXTURE_2D, i_glResource);
	GLenum err = glGetError();
	if(err != GL_NO_ERROR) 
	{
		ILog::Error("CImage::CreateFromTexture2D unable to set param like texture 2D: %s", (char *)gluErrorString(err) );
		return NULL;
	}
	int width,height,intformat,format;
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	if(width == 0 || height == 0) 
	{
		ILog::Error("CImage::CreateFromTexture2D, uninitialized texture" );
		return NULL;
	}
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &intformat);
	CImage *pImage = CreateFromOpenGLParam(width,height,intformat,format);
	if(pImage == NULL) 
	{
		ILog::Error("CImage::CreateFromTexture2D, unsupported internal format" );
		return NULL;
	}

	glGetTexImage(GL_TEXTURE_2D,0,format,GL_UNSIGNED_BYTE, pImage->GetBits() );
	glBindTexture(GL_TEXTURE_2D,0);
	return pImage;
}

#pragma endregion

#pragma region Gamma correction
//**********************
//** GAMMA CORRECTION **
//**********************
void CImage::Linearize()
{
	unsigned char* pDest = m_pBits;
	int n = m_iBpp<=2? 1 : 3;
	for(unsigned int x=0;x<m_iDimx;x++)
		for(unsigned int y=0;y<m_iDimy;y++)
		{
			for(int i=0;i<n;i++)
			{
				float v = pDest[i]/255.f;
				v = powf(v,2.2f);
				pDest[i] = (unsigned char)(v * 255.f);

			}
			pDest+=m_iBpp;
		}
}

void CImage::Delinearize()
{
	unsigned char* pDest = m_pBits;
	int n = m_iBpp<=2? 1 : 3;
	for(unsigned int x=0;x<m_iDimx;x++)
		for(unsigned int y=0;y<m_iDimy;y++)
		{
			for(int i=0;i<n;i++)
			{
				float v = pDest[i]/255.f;
				v = powf(v,1/2.2f);
				pDest[i] = (unsigned char)(v * 255.f);

			}
			pDest+=m_iBpp;
		}
}
#pragma endregion

#pragma region File support

//******************
//** FILE SUPPORT **
//******************
CImage* CImage::LoadFile(const char* i_sPath,bool bIgnoreExtension)
{
	std::list<IImageLoader*>::iterator i;
	if( !bIgnoreExtension )
	{
		const char* extension = strrchr(i_sPath,'.');
		if( extension == NULL )
		{
			ILog::Message("WARNING: CImage::CreateFromFile extension not found on namefile, trying all registed loader\n"); 
		} else
		{
			extension++; // skip dot
			for( i=s_lpLoaders.begin(); i!=s_lpLoaders.end(); ++i)
			{
				if( (*i)->IsSupported(extension) )
				{
					CImage *r = (*i)->Load( i_sPath );
					if( r )
					{
						ILog::Message("\"%s\" loaded by \"%s\": %ix%ix%i\n", i_sPath, (*i)->GetName(),
							r->GetDimx(),r->GetDimy(),r->GetBits() );
						return r;
					}
				}
			}
			ILog::Message("WARNING: unable to load \"%s\" with extension, trying all loaders...\n", i_sPath);
		}
	}
	for( i=s_lpLoaders.begin(); i!=s_lpLoaders.end(); ++i)
	{
		CImage *r = (*i)->Load( i_sPath );
		if( r )
		{
			ILog::Message("\"%s\" loaded by \"%s\": %ix%ix%i\n", i_sPath, (*i)->GetName(),
				r->GetDimx(),r->GetDimy(),r->GetBits() );
			return r;
		}
	}
	ILog::Error("unable to load \"%s\" with registed loaders.\n", i_sPath);
	return NULL;
}

bool CImage::Save(const char* i_sPath,IImageLoader* i_pLoader)
{
	if( i_pLoader!= NULL )
	{
		if( i_pLoader->Save(i_sPath,this) )
		{
			ILog::Message("\"%s\" saved by \"%s\" as requested\n", i_sPath, i_pLoader->GetName() );
			return true;
		}
		ILog::Message("WARNING: unable to save \"%s\" with requested loader \"%s\". Trying all registed loaders",
			i_sPath, i_pLoader->GetName());
	}
	std::list<IImageLoader*>::iterator i;
	const char* extension = strrchr(i_sPath,'.');
	if( extension != NULL )
	{
		extension++; // skip dot
		for( i=s_lpLoaders.begin(); i!=s_lpLoaders.end(); ++i)
		{
			if( (*i)->IsSupported(extension) )
			{
				if( (*i)->Save(i_sPath,this) )
				{
					ILog::Message("\"%s\" saved by \"%s\"\n", i_sPath, (*i)->GetName() );
					return true;
				}
			}
		}
		ILog::Message("WARNING: unable to save \"%s\" by extension, brute trying all loaders...\n");
	}
	for( i=s_lpLoaders.begin(); i!=s_lpLoaders.end(); ++i)
	{
		if( (*i)->Save(i_sPath,this) )
		{
			ILog::Message("\"%s\" saved by \"%s\"\n", i_sPath, (*i)->GetName() );
			return true;
		}
	}
	ILog::Error("unable to save \"%s\"\n",i_sPath);
	return false;
}

void CImage::RegisterLoader(IImageLoader* i_pLoader)
{
	if( std::find(s_lpLoaders.begin(),s_lpLoaders.end(),i_pLoader) == s_lpLoaders.end() )
	{
		s_lpLoaders.push_front(i_pLoader);
		ILog::Message( "Registed image loader \"%s\"\n", i_pLoader->GetName() );
	} else
	{
		ILog::Error("Image loader \"%s\" already registed\n", i_pLoader->GetName() );
	}
}

void CImage::UnregisterLoader(IImageLoader* i_pLoader)
{
	std::list<IImageLoader*>::iterator i = 
		std::find(s_lpLoaders.begin(),s_lpLoaders.end(),i_pLoader);
	if( i != s_lpLoaders.end() )
	{
		ILog::Message( "Unregisted image loader \"%s\"\n", i_pLoader->GetName() );
 	} else
	{
		ILog::Error("Image loader \"%s\" not found, while unregister.\n", i_pLoader->GetName() );
	}
}

std::list<IImageLoader*> CImage::s_lpLoaders;

CImage* IImageLoader::CreateInEngine() { return new CImage(); }

#pragma endregion
