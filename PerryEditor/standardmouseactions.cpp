#include "standardmouseactions.h"

#include "mainwindow.h"
#include "mouseactions.h"
#include "mouseselect.h"
#include "mousemove.h"
#include "mouserotate.h"

#include <QApplication>
#include <QToolBar>
#include <QComboBox>

CStandardMouseActions::CStandardMouseActions()
{
}

void CStandardMouseActions::InitActions()
{
	MainWindow *pMain = MainWindow::Instance();
	m_qToolbar = new QToolBar(pMain);
	m_qToolbar->setObjectName(QString::fromUtf8("standard mouse"));
	m_qToolbar->setWindowTitle(QApplication::translate("MouseActions", "Standard mouse", 0, QApplication::UnicodeUTF8));
	pMain->addToolBar(Qt::TopToolBarArea, m_qToolbar);

	m_pMouseSelect = new CMouseSelect(pMain);
	CMouseActions::AddAction(m_pMouseSelect);
	m_qToolbar->addAction(m_pMouseSelect);

	m_pMouseMove = new CMouseMove(pMain);
	CMouseActions::AddAction(m_pMouseMove);
	m_qToolbar->addAction(m_pMouseMove);

	m_pMouseRotate = new CMouseRotate(pMain);
	CMouseActions::AddAction(m_pMouseRotate);
	m_qToolbar->addAction(m_pMouseRotate);

	m_pMouseSelect->setChecked(true);
	CMouseActions::SetDefault(m_pMouseSelect);

	m_qToolbar->addSeparator();

	m_qCombo = new QComboBox(m_qToolbar);
	m_qCombo->insertItem(0,QApplication::translate("MouseActions", "world", 0, QApplication::UnicodeUTF8));
	m_qCombo->insertItem(1,QApplication::translate("MouseActions", "local", 0, QApplication::UnicodeUTF8));
	m_qCombo->insertItem(2,QApplication::translate("MouseActions", "camera", 0, QApplication::UnicodeUTF8));
	m_qToolbar->addWidget(m_qCombo);
}
