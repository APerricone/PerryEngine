#ifndef QLOG_H
#define QLOG_H

#include <QDockWidget>
#include "log.h"

namespace Ui {
    class QLog;
}

class QLog : public QDockWidget, public ILog
{
    Q_OBJECT

public:
    explicit QLog(QWidget *parent = 0);
    ~QLog();

	QString toHtml() const;
protected:
	virtual void InternalMessage(const char *i_pMessage, va_list args);
	virtual void InternalError(const char *i_pMessage, va_list args);

private slots:
	void on_m_qCopyBtn_clicked();
	void on_m_qClearBtn_clicked();

private:
    Ui::QLog *ui;
	QString m_strBuff;	
	bool m_bClearNext;

	void ManageBuff();
};

#endif // QLOG_H
