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

	connect(ui->m_qDiffuseColor, SIGNAL(ColorChanged(QColor)),
			this, SLOT(SetDiffuse(QColor)));
	connect(ui->m_qSpecularColor, SIGNAL(ColorChanged(QColor)),
			this, SLOT(SetSpecular(QColor)));
	bSet = false;
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
		bSet=true;
		const CMaterial::Data& oMat = m_pModel->GetMaterial(0);
		ui->m_qDiffuseColor->SetColor( QColor((int)(255*oMat.f4Color.x()),
									  (int)(255*oMat.f4Color.y()),
									  (int)(255*oMat.f4Color.z())));
		ui->m_qSpecularColor->SetColor( QColor(
									  (int)(255*oMat.f4Specular.x()),
									  (int)(255*oMat.f4Specular.y()),
									  (int)(255*oMat.f4Specular.z())));
		ui->m_qAlphaValue->setValue( oMat.f4Color.w() * 100);
		ui->m_qSpecularLevel->setValue( oMat.f4Specular.w() * 250);
		bSet=false;
	}
}


void QMaterialEditor::SetDiffuse(const QColor& newDiffuse)
{
	if( bSet ) return;
	bSet = true;
	CMaterial::Data oMat = m_pModel->GetMaterial(0);
	oMat.f4Color.x() = newDiffuse.redF();
	oMat.f4Color.y() = newDiffuse.greenF();
	oMat.f4Color.z() = newDiffuse.blueF();
	ui->m_qDiffuseColor->SetColor( QColor(newDiffuse.red(),
								  newDiffuse.green(),
								  newDiffuse.blue()));
	m_pModel->SetMaterial(0, oMat);
	bSet = false;
}

void QMaterialEditor::SetSpecular(const QColor& newSpecular)
{
	if( bSet ) return;
	bSet = true;
	CMaterial::Data oMat = m_pModel->GetMaterial(0);
	oMat.f4Specular.x() = newSpecular.redF();
	oMat.f4Specular.y() = newSpecular.greenF();
	oMat.f4Specular.z() = newSpecular.blueF();
	ui->m_qSpecularColor->SetColor( QColor(newSpecular.red(),
								  newSpecular.green(),
								  newSpecular.blue()));
	m_pModel->SetMaterial(0, oMat);
	bSet = false;
}

void QMaterialEditor::on_m_qSpecularLevel_valueChanged(int val)
{
	CMaterial::Data oMat = m_pModel->GetMaterial(0);
	oMat.f4Specular.w() = val/250.f;
	m_pModel->SetMaterial(0, oMat);
}
