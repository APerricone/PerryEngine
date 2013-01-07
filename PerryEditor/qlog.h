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

protected:
	virtual void InternalMessage(const char *i_pMessage, va_list args);
	virtual void InternalError(const char *i_pMessage, va_list args);

private slots:
	void on_actionClear_triggered();

	void on_actionCopy_triggered();

private:
    Ui::QLog *ui;
	char m_strBuff[1024];

	void ManageBuff();
};

#endif // QLOG_H
