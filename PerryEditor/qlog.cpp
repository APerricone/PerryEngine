#include "qlog.h"
#include "ui_qlog.h"
#include <stdio.h>

#include <QApplication>

QLog::QLog(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::QLog)
{
    ui->setupUi(this);
	ILog::SetLog(this);
	m_bClearNext = false;
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
	if( m_bClearNext || ( m_strBuff.left(1)=="\r" ))
	{
		oCursor.select(QTextCursor::LineUnderCursor);
		oCursor.removeSelectedText();
		ui->m_qText->setTextCursor(oCursor);
		oCursor = ui->m_qText->textCursor();
	}
	m_strBuff.replace("\n","<br>");
	m_strBuff.replace("\t","&#9;");

	QStringList l = m_strBuff.split("\r", QString::SkipEmptyParts);;
	oCursor.insertHtml(l.last());
	m_bClearNext = ( m_strBuff.right(1)=="\r" );
	ui->m_qText->setTextCursor(oCursor);
	qApp->processEvents();
}

QString QLog::toHtml() const
{
	return ui->m_qText->toHtml();
}

void QLog::on_m_qCopyBtn_clicked()
{
	static int i=0;
	switch(i)
	{
	case 0: ILog::Message("\rTest 123"); break;
	case 1: ILog::Message("\r123 Test"); break;
	}
	i=1-i;

	/*
	QTextCursor oCursor = ui->m_qText->textCursor();
	oCursor.select(QTextCursor::Document);
	ui->m_qText->copy();
	oCursor.clearSelection();
	oCursor.movePosition(QTextCursor::End);
	ui->m_qText->setTextCursor(oCursor);*/
}

void QLog::on_m_qClearBtn_clicked()
{
	ui->m_qText->clear();
}
