#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class CPerryView;
class QLog;
class QRenderingOptions;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

	static MainWindow *s_pInstance;
public:
	static MainWindow* Instance() { return s_pInstance; }
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	bool Check();

	void OpenGLInitialized();
private slots:
	void on_actionAbout_Qt_triggered();
	void on_actionExit_triggered();
	void on_action_About_triggered();

	void on_actionSave_screenshot_triggered();

private:
    Ui::MainWindow *ui;
	CPerryView *m_qMainView;
	QLog *m_qLog;
	QRenderingOptions *m_qRenderingOptions;

protected:
	virtual void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H
