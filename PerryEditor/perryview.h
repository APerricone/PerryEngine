#ifndef PERRYVIEW_H
#define PERRYVIEW_H

#include "opengl.h"
#include <QGLWidget>
#include "scene.h"

class CEditorCamera;
class QBasicTimer;
class QToolBar;
class CStandardMouseActions;

class IRenderingOptions
{
public:
	virtual bool GetSSAOEnabled() const = 0;
	virtual bool GetDiffuseEnabled() const = 0;
	virtual bool GetSpecularEnabled() const = 0;
	virtual bool GetColorEnabled() const = 0;
	virtual bool GetFXAAEnabled() const = 0;
	virtual bool GetGridEnabled() const = 0;

	virtual bool GetPostChanged() const =0;
	virtual bool GetNylonEnabled() const =0;
	virtual bool GetExposureEnabled() const =0;
	virtual bool GetFrameEnabled() const =0;
	virtual bool GetLutEnabled() const =0;

};

class CPerryView : public QGLWidget
{
    Q_OBJECT
public:
	explicit CPerryView(QWidget * parent = 0);
	~CPerryView();

	// Custom GL "Print" Routine
	static void StdGlPrint(float x,float y,const char *fmt, ...);

	void SetRenderingOptions(IRenderingOptions* p) { m_qRenderingOptions = p; }

	bool IsInitialized() const { return m_bIsInitialized; }

	QToolBar* GetStandardMouseActionsToolbar();

	void SaveScreenshot(const QString& fileName,const int2& size = int2(0,0), bool lut = false);
	void LoadLut(QString i_qPath);
signals:

public slots:

protected:
	virtual void initializeGL();
	virtual void paintGL();
	virtual void resizeGL( int width, int height );
	virtual void wheelEvent( QWheelEvent * event );
	virtual void keyPressEvent ( QKeyEvent * event );
	virtual void keyReleaseEvent ( QKeyEvent * event );
	virtual void mouseMoveEvent( QMouseEvent * event );
	virtual void mousePressEvent( QMouseEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * event );
	virtual void timerEvent(QTimerEvent *e);


	QBasicTimer *m_qTimer;
	CScene *m_pScene;
	CEditorCamera *m_pCamera;
	IRenderingOptions *m_qRenderingOptions;
	CStandardMouseActions *m_pStandardMouseActions;
	bool m_bIsInitialized;
	bool m_bScreenshotMode;
private:
	static CPerryView* s_pCurrentView;

	// callback for CScene
	static void DrawEditor(void*);

	void EvidenceSelection();
	void DrawGrid();
	void DrawBackground();


	// temp
	void SampleScene();
};

#endif // PERRYVIEW_H
