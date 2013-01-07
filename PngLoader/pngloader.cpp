#include "pngloader.h"
#include "stdio.h"

#include "png.h"
#include "log.h"

CPngLoader CPngLoader::loader;

void CPngLoader::Register()
{
	CImage::RegisterLoader(&loader);
}

void CPngLoader::Unregister()
{
	CImage::UnregisterLoader(&loader);
}

bool CPngLoader::IsSupported(const char *i_sExtension)
{
	return _stricmp(i_sExtension,"png") == 0;
}

void my_read(png_structp png_ptr,unsigned char *dest,unsigned int n)
{
	fread(dest,n,1,(FILE*)png_ptr->io_ptr);
}

CImage* CPngLoader::Load(const char *i_sPath)
{
	ILog::Message("Loading %s...",i_sPath);
	FILE *fp = fopen(i_sPath, "rb");
	if (!fp)
	{ 
		ILog::Error("not found\n"); 
		return NULL; 
	}
#define NUM_HEADER 8
	unsigned char header[NUM_HEADER];
	fread(header, 1, NUM_HEADER, fp);
	bool is_png = !png_sig_cmp(header, 0, NUM_HEADER);
	if (!is_png)
	{
		ILog::Error("not a valid png\n"); 
		fclose(fp); 
		return NULL; 
	}
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	png_infop info_ptr=png_create_info_struct(png_ptr);
	png_infop end_ptr=png_create_info_struct(png_ptr);

	png_init_io(png_ptr, fp);
	png_set_read_fn(png_ptr, fp, my_read);
	png_set_sig_bytes(png_ptr, NUM_HEADER);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	if(info_ptr->bit_depth!=8)
	{
		ILog::Error("unsupported format (only 8bit images are supported)\n"); 
		png_destroy_read_struct(&png_ptr,&info_ptr,0); 
		fclose(fp); 
		return NULL; 
	}

	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
	CImage *pImage = CreateInEngine();
	pImage->Create(info_ptr->width,info_ptr->height,info_ptr->channels);
	unsigned char* pBits = pImage->GetBits();
	for(unsigned int y=0;y<info_ptr->height;y++)
		memcpy(
			&pBits[y*info_ptr->width*info_ptr->channels],
			row_pointers[info_ptr->height-1-y],
			info_ptr->width*info_ptr->channels);

	png_destroy_read_struct(&png_ptr,&info_ptr,&end_ptr);
	fclose(fp);
	ILog::Message("OK\n",i_sPath);
	return pImage;
}

void my_write(png_structp png_ptr,unsigned char *dest,unsigned int n)
{
	fwrite(dest,n,1,(FILE*)png_ptr->io_ptr);
}

bool CPngLoader::Save(const char *i_sPath,const CImage* i_pImage)
{
	FILE *fp = fopen(i_sPath, "wb");
	if (!fp)
	{ 
		ILog::Error("unable to open file\n"); 
		return false; 
	}
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));
	png_init_io(png_ptr, fp); 
	png_set_write_fn(png_ptr, fp, my_write,NULL);

	int ct;
	switch(i_pImage->GetBpp())
	{
	case 1: ct = PNG_COLOR_TYPE_GRAY; break;
	case 2: ct = PNG_COLOR_TYPE_GA; break;
	case 3: ct = PNG_COLOR_TYPE_RGB; break;
	case 4: ct = PNG_COLOR_TYPE_RGBA; break;
	}
	png_set_IHDR(png_ptr, info_ptr, i_pImage->GetDimx(), i_pImage->GetDimy(),
			8, ct, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);	

	for (unsigned int y=0 ; y<i_pImage->GetDimy(); y++)
	{
		png_write_row(png_ptr, &i_pImage->GetBits()[(i_pImage->GetDimy()-1-y)*i_pImage->GetStride()]);
	}
	png_write_end(png_ptr, NULL);
	fclose(fp);
	png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	ILog::Message("OK\n",i_sPath);
	return true;
}
