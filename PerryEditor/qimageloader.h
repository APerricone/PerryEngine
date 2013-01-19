#ifndef QIMAGELOADER_H
#define QIMAGELOADER_H

#include "image.h"

class QByteArray;
template <typename T> class QList;

class QImageLoader : public IImageLoader
{
	static QImageLoader loader;
public:
	static void Register();
	static void Unregister();

	virtual const char* GetName() { return "Qt image loader"; }

	virtual bool CanLoad(const char *i_sExtension);
	virtual CImage* Load(const char *i_sPath);

	virtual bool CanSave(const char *i_sExtension);
	virtual bool Save(const char *i_sPath,const CImage* i_pImage);
private:
	bool CheckIsInList(const char *i_sExtension, const QList<QByteArray>& i_qList);
};

#endif // QIMAGELOADER_H
