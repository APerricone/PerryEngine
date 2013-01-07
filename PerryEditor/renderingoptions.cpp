#include "renderingoptions.h"
#include "ui_renderingoptions.h"

QRenderingOptions::QRenderingOptions(QWidget *parent) :
    QDockWidget(parent),
	ui(new Ui::QRenderingOptions),
	m_bSSAOEnabled(true),
	m_bDiffuseEnabled(true),
	m_bSpecularEnabled(true),
	m_bColorEnabled(true),
	m_bFXAAEnabled(true)
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
}
