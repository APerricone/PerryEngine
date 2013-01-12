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

private slots:

	void on_actionUpdateLayers_triggered();

private:
	Ui::QRenderingOptions *ui;

	bool m_bSSAOEnabled;
	bool m_bDiffuseEnabled;
	bool m_bSpecularEnabled;
	bool m_bColorEnabled;
	bool m_bFXAAEnabled;
	bool m_bGridEnabled;
};

#endif // GAMMAOPTIONS_H
