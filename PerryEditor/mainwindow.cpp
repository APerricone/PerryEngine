#include "opengl.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "perryview.h"
#include "qlog.h"
#include "renderingoptions.h"
#include "Engine_config.h"
#include "mouseactions.h"
#include "savescreenshot.h"
#include "materialeditor.h"

#include <QSettings>
#include <QMessageBox>
#include <QPushButton>
#include <QGridLayout>

MainWindow *MainWindow::s_pInstance;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	s_pInstance = this;
	ui->setupUi(this);

	CMouseActions::Init(this,ui->m_qMenuTools);

	m_qMainView = new CPerryView(this);
	setCentralWidget(m_qMainView);

	m_qLog = new QLog(this);
	addDockWidget(Qt::BottomDockWidgetArea,m_qLog);
	ui->m_qMenuView->addAction(m_qLog->toggleViewAction());

	m_qMaterialEditor = new QMaterialEditor(this);
	addDockWidget(Qt::RightDockWidgetArea,m_qMaterialEditor);
	ui->m_qMenuView->addAction(m_qMaterialEditor->toggleViewAction());

	m_qRenderingOptions = new QRenderingOptions(this);
	addDockWidget(Qt::RightDockWidgetArea,m_qRenderingOptions);
	ui->m_qMenuView->addAction(m_qRenderingOptions->toggleViewAction());
	m_qMainView->SetRenderingOptions(m_qRenderingOptions);

	QSettings settings("Perry", "PerryEditor");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::OpenGLInitialized()
{
	ui->m_qToolbarsMenu->addAction(ui->mainToolBar->toggleViewAction());
	ui->m_qToolbarsMenu->addAction(m_qMainView->GetStandardMouseActionsToolbar()->toggleViewAction());
}

bool MainWindow::Check()
{
	if(!m_qMainView->IsInitialized())
	{
		m_qLog->setVisible(true);
		QString message("Unable to initialize engine<br><br>");
		message += m_qLog->toHtml();
		QMessageBox::critical(this,"Perry's Editor",message);
		close();
		return false;
	}
	return true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_Qt_triggered()
{
	qApp->aboutQt();
}

void MainWindow::on_actionExit_triggered()
{
	close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QSettings settings("Perry", "PerryEditor");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	QMainWindow::closeEvent(event);
}

void MainWindow::on_action_About_triggered()
{
	QString message("");
	// TITLE
	message += "<center>";
	message += "<h1>Perry's Editor</h1>";
	message += "Editor v.0.1<br>";
	message += "Engine v." Engine_VER_FILEVERSION_STR "<br>";
	message += "by <a href=\"http://amicoperry.altervista.org\">Antonino Perricone</a><br>";
	message += "</center>";
	// EXTRA HELP
	message += tr("<h2>Camera commands</h2>");
	message += tr("<b>Mouse wheel</b>: move the camera forward/backwards<br>");
	message += tr("<b>CTRL + Mouse wheel</b>: change fov of camera<br>");
	message += tr("<b>Middle mouse button + move</b>: move the camera<br>");
	message += tr("<b>CTRL + Middle mouse button + move</b>: rotate around selection<br>");
	message += tr("<h2>Selection</h2>");
	message += tr("<b>Left mouse button</b>: Select object under mouse<br>");
	/*TODO*/ message += "<s>";
	message += tr("<b>CRTL + Left mouse button</b>: box selection<br>");
	/*TODO*/ message += "</s>";
	message += tr("<b>SHIFT + one of two commands above</b>: add to selection<br>");
	message += tr("<b>ALT + one of two commands above</b>: remove to selection<br>");
	message += tr("<p><b>NOTE:</b> These commands are valid when there is not a gizmo actived.<br>");
	QMessageBox::about(this,"Perry's Editor",message);
}

void MainWindow::on_actionSave_screenshot_triggered()
{
	QSaveScreenshot file(this);
	if(file.exec() == QDialog::Accepted)
	{
		m_qMainView->SaveScreenshot(
					file.selectedFiles()[0],
					file.GetCurrentSize(),
					file.IsLutEnabled()	);
	}
}

void MainWindow::LoadLut(QString i_qPath)
{
	m_qMainView->LoadLut(i_qPath);
}
