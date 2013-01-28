#ifndef QMATERIALEDITOR_H
#define QMATERIALEDITOR_H

#include <QDockWidget>
#include <QList>

class CNode;
class CModel;

namespace Ui {
class QMaterialEditor;
}

class QMaterialEditor : public QDockWidget
{
	Q_OBJECT
	
public:
	explicit QMaterialEditor(QWidget *parent = 0);
	~QMaterialEditor();
	
public slots:
	void selectionChanged(const QList<CNode*>& list);
	void SetDiffuse(const QColor& newDiffuse);
	void SetSpecular(const QColor& newSpecular);

private slots:
	void on_m_qSpecularLevel_valueChanged(int arg1);

private:
	Ui::QMaterialEditor *ui;
	CModel *m_pModel;
	bool bSet;
};

#endif // QMATERIALEDITOR_H
