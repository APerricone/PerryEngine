#ifndef GAMMAOPTIONS_H
#define GAMMAOPTIONS_H

#include <QDockWidget>
#include "perryview.h"

namespace Ui {
	class QRenderingOptions;
}

class QRenderingOptions : public QDockWidget, public IRenderingOptions
{
    Q_OBJECT

public:
	explicit QRenderingOptions(QWidget *parent = 0);
	~QRenderingOptions();

	virtual bool GetSSAOEnabled() const { return m_bSSAOEnabled; }
	virtual bool GetDiffuseEnabled() const { return m_bDiffuseEnabled; }
	virtual bool GetSpecularEnabled() const { return m_bSpecularEnabled; }
	virtual bool GetColorEnabled() const { return m_bColorEnabled; }
	virtual bool GetFXAAEnabled() const { return m_bFXAAEnabled; }
	virtual bool GetGridEnabled() const { return m_bGridEnabled; }

	virtual bool GetPostChanged() const { bool b(m_bPostChanged); m_bPostChanged= false; return b; }
	virtual bool GetNylonEnabled() const { return m_bNylon; }
	virtual bool GetExposureEnabled() const { return m_bExposure; }
	virtual bool GetFrameEnabled() const { return m_bFrame; }
	virtual bool GetLutEnabled() const { return m_bLut; }
private slots:

	void on_actionUpdateLayers_triggered();

	void on_actionUpdate_Post_triggered();

	void on_pushButton_2_clicked();

private:
	Ui::QRenderingOptions *ui;

	bool m_bSSAOEnabled;
	bool m_bDiffuseEnabled;
	bool m_bSpecularEnabled;
	bool m_bColorEnabled;
	bool m_bFXAAEnabled;
	bool m_bGridEnabled;

	mutable bool m_bPostChanged;
	bool m_bNylon;
	bool m_bExposure;
	bool m_bFrame;
	bool m_bLut;
};

#endif // GAMMAOPTIONS_H
