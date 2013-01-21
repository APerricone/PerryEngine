#include "savescreenshot.h"
#include "ui_savescreenshot.h"

#include <QImageWriter>
#include <QGridLayout>
#include "perryview.h"
#include "mainwindow.h"

QString QSaveScreenshot::FillFilter(const QList<QByteArray>& list)
{
	QStringList filters;
	for(int i=0 ; i<list.count() ; i++)
		filters << list[i].toLower();
	filters.sort();

	QString final(tr("Image files"));
	final+=" (";
	for(int i=0 ; i<filters.count() ; i++)
	{
		final +="*."+ filters[i];
		if(i!=filters.count()-1)
			final +=" "; else
			final +=")";
	}

	return final;
}

QSaveScreenshot::QSaveScreenshot(QWidget *parent) :
	QFileDialog(parent),
	ui(new Ui::SaveScreenshot),
	m_bLock(false),
	m_bLut(false)
{
	ui->setupUi(this);
	QGridLayout *layout = (QGridLayout*)QFileDialog::layout();
	int row = layout->rowCount();
	layout->addWidget(ui->m_qExtraOptions,row,0,1,-1);
	layout->setRowMinimumHeight(row,
	ui->m_qExtraOptions->geometry().height());

	setAcceptMode(QFileDialog::AcceptSave);
	setDefaultSuffix("jpg");

	QStringList filters;
	filters << FillFilter(QImageWriter::supportedImageFormats());

	setNameFilters(filters);

	on_m_qPresets_currentIndexChanged(0);

}

QSaveScreenshot::~QSaveScreenshot()
{
	delete ui;
}

void QSaveScreenshot::on_m_qPresets_currentIndexChanged(int index)
{
	if(m_bLock) return;
	m_bLock = true;
	switch(index)
	{
	case 0: //current view
	case 7: //current view x4
	{
		QWidget* pView = MainWindow::Instance()->centralWidget();
		m_i2CurrentSize.x() = pView->width();
		m_i2CurrentSize.y() = pView->height();
		if(index==7)
			m_i2CurrentSize*=2;
	}
		break;
	case 1: m_i2CurrentSize.Set(640,480); break;
	case 2: m_i2CurrentSize.Set(800,600); break;
	case 3: m_i2CurrentSize.Set(1024,768); break;
	case 4: m_i2CurrentSize.Set(1280,720); break;
	case 5: m_i2CurrentSize.Set(1600,1200); break;
	case 6: m_i2CurrentSize.Set(1920,1080); break;
	}
	ui->m_qWidth->setValue( m_i2CurrentSize.x() );
	ui->m_qHeight->setValue( m_i2CurrentSize.y() );
	m_fAspectRatio = float(m_i2CurrentSize.x()) / float(m_i2CurrentSize.y());
	m_bLock = false;
}

void QSaveScreenshot::on_m_qWidth_valueChanged(int w)
{
	if(m_bLock) return;
	m_bLock = true;
	ui->m_qPresets->setCurrentIndex(8);
	m_i2CurrentSize.x() = w;
	if(ui->m_qLock->isChecked())
	{
		m_i2CurrentSize.y() = w / m_fAspectRatio;
		ui->m_qHeight->setValue( m_i2CurrentSize.y() );
	} else
	{
		m_fAspectRatio = float(w) / float(ui->m_qHeight->value());
	}
	m_bLock = false;
}

void QSaveScreenshot::on_m_qHeight_valueChanged(int h)
{
	if(m_bLock) return;
	m_bLock = true;
	ui->m_qPresets->setCurrentIndex(8);
	m_i2CurrentSize.y() = h;
	if(ui->m_qLock->isChecked())
	{
		m_i2CurrentSize.x() = h * m_fAspectRatio;
		ui->m_qWidth->setValue( m_i2CurrentSize.x() );
	} else
	{
		m_fAspectRatio = float(ui->m_qWidth->value()) / float(h);
	}
	m_bLock = false;
}

void QSaveScreenshot::on_m_qLutCheck_clicked(bool checked)
{
	m_bLut = checked;
}
