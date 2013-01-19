#ifndef SAVESCREENSHOT_H
#define SAVESCREENSHOT_H

#include "vector.h"
#include <QFileDialog>

namespace Ui {
class SaveScreenshot;
}

class QSaveScreenshot : public QFileDialog
{
	Q_OBJECT
	
public:
	explicit QSaveScreenshot(QWidget *parent = 0);
	~QSaveScreenshot();
	
	QString FillFilter(const QList<QByteArray> &list);

	 const int2& GetCurrentSize() const { return m_i2CurrentSize; }
	 bool IsLutEnabled() const { return m_bLut; }

private slots:
	void on_m_qPresets_currentIndexChanged(int index);

	void on_m_qWidth_valueChanged(int arg1);

	void on_m_qHeight_valueChanged(int arg1);

	void on_m_qLutCheck_clicked(bool checked);

private:
	Ui::SaveScreenshot *ui;
	int2 m_i2CurrentSize;
	float m_fAspectRatio;
	bool m_bLock;
	bool m_bLut;
};

#endif // SAVESCREENSHOT_H
