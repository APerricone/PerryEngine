#include "renderingoptions.h"
#include "ui_renderingoptions.h"

QRenderingOptions::QRenderingOptions(QWidget *parent) :
    QDockWidget(parent),
	ui(new Ui::QRenderingOptions),
	m_bSSAOEnabled(true),
	m_bDiffuseEnabled(true),
	m_bSpecularEnabled(true),
	m_bColorEnabled(true),
	m_bFXAAEnabled(true),
	m_bGridEnabled(true)
{
    ui->setupUi(this);

}

QRenderingOptions::~QRenderingOptions()
{
    delete ui;
}



void QRenderingOptions::on_actionUpdateLayers_triggered()
{
	m_bSSAOEnabled = ui->m_qSSAOCheck->isChecked();
	m_bDiffuseEnabled = ui->m_qDiffuseCheck->isChecked();
	m_bSpecularEnabled = ui->m_qSpecularCheck->isChecked();
	m_bColorEnabled = ui->m_qColorCheck->isChecked();
	m_bFXAAEnabled = ui->m_qFXAACheck->isChecked();
	m_bGridEnabled = ui->m_qGridCheck->isChecked();
}

void QRenderingOptions::on_actionUpdate_Post_triggered()
{
	m_bPostChanged = true;
	m_bNylon = ui->m_qNylon->isChecked();
	m_bExposure = ui->m_qExposure->isChecked();
	m_bFrame = ui->m_qFrame->isChecked();
	m_bLut = ui->m_qLut->isChecked();
}
