#include "qimageloader.h"
#include <QImage>
#include "log.h"
#include <QImageWriter>
#include <QImageReader>

QImageLoader QImageLoader::loader;

void QImageLoader::Register()
{
	CImage::RegisterLoader(&loader);
}

void QImageLoader::Unregister()
{
	CImage::RegisterLoader(&loader);
}

bool QImageLoader::IsSupported(const char *i_sExtension)
{
	QList<QByteArray> l = QImageWriter::supportedImageFormats();
	QList<QByteArray>::Iterator i;
	for(i=l.begin();i!=l.end();++i)
	{
		if( qstricmp(*i,i_sExtension)==0 )
			return true;
	}
	l = QImageReader::supportedImageFormats();
	for(i=l.begin();i!=l.end();++i)
	{
		if( qstricmp(*i,i_sExtension)==0 )
			return true;
	}
	return false;
}

CImage* QImageLoader::Load(const char *i_sPath)
{
	ILog::Message("Loading %s...",i_sPath);
	QImage tmp(i_sPath);
	if( tmp.isNull() )
	{
		ILog::Message("Error");
		return 0;
	}

	CImage *pImage = CreateInEngine();
	unsigned int bpp;
	bpp = tmp.isGrayscale()? 1 : 3;
	bpp += tmp.hasAlphaChannel()? 1 : 0;
	pImage->Create(tmp.width(),tmp.height(),bpp);

	QImage rgba = tmp.convertToFormat(QImage::Format_ARGB32);
	unsigned char* dest = pImage->GetBits();
	QRgb *source;
	for(int y=0;y<tmp.height();y++)
	{
		source = (QRgb*)(rgba.scanLine(y));
		for(int x=0;x<tmp.width();x++)
		{
			switch(bpp)
			{
			case 1:
				dest[0]=qRed(*source);
				break;
			case 2:
				dest[0]=qRed(*source);
				dest[1]=qAlpha(*source);
				break;
			case 3:
				dest[0]=qRed(*source);
				dest[1]=qGreen(*source);
				dest[2]=qBlue(*source);
				break;
			case 4:
				dest[0]=qRed(*source);
				dest[1]=qGreen(*source);
				dest[2]=qBlue(*source);
				dest[3]=qAlpha(*source);
				break;
			}
			dest+=bpp;
			source+=4;
		}
	}
	ILog::Message("OK\n");
	return pImage;
}

bool QImageLoader::Save(const char *i_sPath,const CImage* i_pImage)
{
	QImage::Format f;
	int bpp = i_pImage->GetBpp();
	switch(bpp)
	{
	case 1: f = QImage::Format_Indexed8; break;
	case 3: f = QImage::Format_RGB32; break;
	case 2:
	case 4: f = QImage::Format_ARGB32; break;
	default:
		return false;
	}

	QImage tmp(i_pImage->GetDimx(),i_pImage->GetDimy(),f);
	if( bpp == 1 )
	{
		QVector<QRgb> cc;
		cc.resize(256);
		for(int i=0;i<256;i++)
			cc[i] = qRgb(i,i,i);
		tmp.setColorTable(cc);
	}

	unsigned char* source = i_pImage->GetBits();
	uchar *dest;
	for(int y=0;y<tmp.height();y++)
	{
		dest = tmp.scanLine(y);
		for(int x=0;x<tmp.width();x++)
		{
			switch(bpp)
			{
			case 1:
				*dest=*source;
				break;
			case 2:
				*((QRgb*)dest) = qRgba(source[0],source[0],source[0],source[1]);
				break;
			case 3:
				*((QRgb*)dest) = qRgb(source[0],source[1],source[2]);
				break;
			case 4:
				*((QRgb*)dest) = qRgba(source[0],source[1],source[2],source[3]);
				break;
			}
			source+=bpp;
			dest+=tmp.depth();
		}
	}

	return tmp.save(i_sPath);
}

