#include "materialeditor.h"
#include "ui_materialeditor.h"

#include "selection.h"
#include "node.h"
#include "model.h"
#include "material.h"

#include <QColorDialog>

QMaterialEditor::QMaterialEditor(QWidget *parent) :
	QDockWidget(parent),
	ui(new Ui::QMaterialEditor)
{
	ui->setupUi(this);
	ui->dockWidgetContents->setEnabled(false);

	connect(CSelection::Instance(), SIGNAL(selectionChanged(const QList<CNode*>&)),
			this, SLOT(selectionChanged(const QList<CNode*>&)));
}

QMaterialEditor::~QMaterialEditor()
{
	delete ui;
}

void QMaterialEditor::selectionChanged(const QList<CNode*>& list)
{
	bool bEnable(false);
	m_pModel=0;
	if( list.size() == 1 )
	{
		if(CModel::IsCModel(*list[0]))
		{
			m_pModel = (CModel*)list[0];
			if(m_pModel->GetMeshNum()==1)
				bEnable=true;
		}
	}

	ui->dockWidgetContents->setEnabled(bEnable);

	if(bEnable)
	{
		const CMaterial::Data& oMat = m_pModel->GetMaterial(0);
		ui->m_qDiffuseColor->setStyleSheet(
					QString().sprintf("background-color: rgb(%i, %i, %i);",
									  (int)(255*oMat.f4Color.x()),
									  (int)(255*oMat.f4Color.y()),
									  (int)(255*oMat.f4Color.z())));
		ui->m_qSpecularColor->setStyleSheet(
					QString().sprintf("background-color: rgb(%i, %i, %i);",
									  (int)(255*oMat.f4Specular.x()),
									  (int)(255*oMat.f4Specular.y()),
									  (int)(255*oMat.f4Specular.z())));
		ui->m_qAlphaValue->setValue( oMat.f4Color.w() * 100);
		ui->m_qSpecularLevel->setValue( oMat.f4Specular.w() * 100);
	} else
	{
		ui->m_qDiffuseColor->setStyleSheet("");
		ui->m_qSpecularColor->setStyleSheet("");
	}
}


void QMaterialEditor::SetDiffuse(const QColor& newDiffuse)
{
	CMaterial::Data oMat = m_pModel->GetMaterial(0);
	oMat.f4Color.x() = newDiffuse.redF();
	oMat.f4Color.y() = newDiffuse.greenF();
	oMat.f4Color.z() = newDiffuse.blueF();
	ui->m_qDiffuseColor->setStyleSheet(
				QString().sprintf("background-color: rgb(%i, %i, %i);",
								  newDiffuse.red(),
								  newDiffuse.green(),
								  newDiffuse.blue()));
	m_pModel->SetMaterial(0, oMat);
}

void QMaterialEditor::on_m_qDiffuseColor_clicked()
{
	CMaterial::Data oMat = m_pModel->GetMaterial(0);
	QColor diff; diff.setRgbF(oMat.f4Color.x(),oMat.f4Color.y(),oMat.f4Color.z());
	QColorDialog dlg(diff,this);
	dlg.setWindowTitle(tr("Select diffuse color"));
	connect(	&dlg,SIGNAL(currentColorChanged(const QColor &)),
				this,SLOT(SetDiffuse(const QColor &)));
	if( dlg.exec() == QDialog::Rejected )
		SetDiffuse(diff);
	disconnect(	&dlg,SIGNAL(currentColorChanged(const QColor &)),
				this,SLOT(SetDiffuse(const QColor &)));
}

void QMaterialEditor::SetSpecular(const QColor& newSpecular)
{
	CMaterial::Data oMat = m_pModel->GetMaterial(0);
	oMat.f4Specular.x() = newSpecular.redF();
	oMat.f4Specular.y() = newSpecular.greenF();
	oMat.f4Specular.z() = newSpecular.blueF();
	ui->m_qSpecularColor->setStyleSheet(
				QString().sprintf("background-color: rgb(%i, %i, %i);",
								  newSpecular.red(),
								  newSpecular.green(),
								  newSpecular.blue()));
	m_pModel->SetMaterial(0, oMat);
}

void QMaterialEditor::on_m_qSpecularColor_clicked()
{
	CMaterial::Data oMat = m_pModel->GetMaterial(0);
	QColor spec; spec.setRgbF(oMat.f4Specular.x(),oMat.f4Specular.y(),oMat.f4Specular.z());
	QColorDialog dlg(spec,this);
	dlg.setWindowTitle(tr("Select specular color"));
	connect(	&dlg,SIGNAL(currentColorChanged(const QColor &)),
				this,SLOT(SetSpecular(const QColor &)));
	if( dlg.exec() == QDialog::Rejected )
		SetSpecular(spec);
	disconnect(	&dlg,SIGNAL(currentColorChanged(const QColor &)),
				this,SLOT(SetSpecular(const QColor &)));
}

void QMaterialEditor::on_m_qSpecularLevel_valueChanged(int val)
{
	CMaterial::Data oMat = m_pModel->GetMaterial(0);
	oMat.f4Specular.w() = val/250.f;
	m_pModel->SetMaterial(0, oMat);
}
