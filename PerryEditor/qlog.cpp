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
	m_strBuff.vsprintf(i_pMessage,args);
	ManageBuff();
}

void QLog::InternalError(const char *i_pMessage, va_list args)
{
	QTextCursor oCursor = ui->m_qText->textCursor();
	oCursor.movePosition(QTextCursor::End);
	oCursor.insertHtml("<p style=\"color:red;font-weight:bold\">ERROR:</p>");
	m_strBuff.vsprintf(i_pMessage,args);
	ManageBuff();
}

void QLog::ManageBuff()
{
	QTextCursor oCursor = ui->m_qText->textCursor();
	oCursor.clearSelection();
	oCursor.movePosition(QTextCursor::End);
	m_strBuff.replace("\n","<br>");
	m_strBuff.replace("\r","<br>");
	m_strBuff.replace("\t","&#9;");
	oCursor.insertHtml(m_strBuff);
	ui->m_qText->setTextCursor(oCursor);
}

QString QLog::toHtml() const
{
	return ui->m_qText->toHtml();
}

void QLog::on_m_qCopyBtn_clicked()
{
	QTextCursor oCursor = ui->m_qText->textCursor();
	oCursor.select(QTextCursor::Document);
	ui->m_qText->copy();
	oCursor.clearSelection();
	oCursor.movePosition(QTextCursor::End);
	ui->m_qText->setTextCursor(oCursor);
}

void QLog::on_m_qClearBtn_clicked()
{
	ui->m_qText->clear();
}
