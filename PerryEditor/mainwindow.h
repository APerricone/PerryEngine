#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class CPerryView;
class QLog;
class QRenderingOptions;
class QMaterialEditor;
class CUndoRedoManager;

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


	void LoadLut(QString i_qPath);

private slots:
	void on_actionAbout_Qt_triggered();
	void on_action_About_triggered();

	void on_actionExit_triggered();
	void on_actionSave_screenshot_triggered();

	void on_actionUndo_triggered();
	void on_actionRedo_triggered();

	void OpenGLInitialized();
	void UndoRedo_Changed(CUndoRedoManager* pUndoRedoManager);

	void on_actionClear_memory_triggered();

	void on_actionWavefront_obj_triggered();

private:
    Ui::MainWindow *ui;
	CPerryView *m_qMainView;
	QLog *m_qLog;
	QRenderingOptions *m_qRenderingOptions;
	QMaterialEditor* m_qMaterialEditor;
	CUndoRedoManager* m_pUndoRedoManager;

protected:
	virtual void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H
