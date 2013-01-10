#include "qlog.h"
#include "ui_qlog.h"
#include <stdio.h>

QLog::QLog(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::QLog)
{
    ui->setupUi(this);
	ILog::SetLog(this);
}

QLog::~QLog()
{
	ILog::SetLog(NULL);
    delete ui;
}

void QLog::InternalMessage(const char *i_pMessage, va_list args)
{
	vsprintf(m_strBuff,i_pMessage,args);
	ManageBuff();
}

void QLog::InternalError(const char *i_pMessage, va_list args)
{
	vsprintf(m_strBuff,i_pMessage,args);
	ManageBuff();
}

void QLog::ManageBuff()
{
	QTextCursor oCursor = ui->m_qText->textCursor();
	oCursor.movePosition(QTextCursor::End);
	char *pBegin = m_strBuff;
	char *pEnd = m_strBuff;
	char c =1;
	do
	{
		c = *pEnd;
		if(	c == '\n' ||
			c == '\r' ||
			c == 0 )
		{
			*pEnd=0;
			oCursor.insertText(pBegin);
			ui->m_qText->ensureCursorVisible();
			pBegin = pEnd+1;
		}

		switch(c)
		{
		case '\n':
			oCursor.insertText("\n");
			break;
		case '\r':
			oCursor.select(QTextCursor::LineUnderCursor);
			oCursor.removeSelectedText();
			break;
		}
		pEnd++;
	} while(c!=0);
}

QString QLog::toHtml() const
{
	return ui->m_qText->toHtml();
}

void QLog::on_actionClear_triggered()
{
	ui->m_qText->clear();
}

void QLog::on_actionCopy_triggered()
{
	QTextCursor oCursor = ui->m_qText->textCursor();
	oCursor.select(QTextCursor::Document);
	ui->m_qText->copy();
	oCursor.clearSelection();
	oCursor.movePosition(QTextCursor::End);
	ui->m_qText->ensureCursorVisible();
}
