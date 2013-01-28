#include "slidercombobox.h"
#include <QSlider>
#include <QStylePainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QStyleOption>

QSliderComboBox::QSliderComboBox(QWidget *parent) :
	QComboBox(parent)
{
	m_qFrame = new QFrame(this,Qt::Popup);
	m_pSlider = new QSlider(Qt::Horizontal,m_qFrame);
	//m_pSlider->overrideWindowFlags(Qt::Popup);
	m_iValue = 0;
	connect(m_pSlider,SIGNAL(valueChanged(int)),
			SLOT(setValue(int)));
	connect(m_pSlider,SIGNAL(sliderReleased()),
			SLOT(hidePopup()));
}

void QSliderComboBox::paintEvent(QPaintEvent *e)
{
	QStylePainter painter(this);
	// draw the combobox frame, focusrect and selected etc.
	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	if( m_qFrame->isVisible() )
		opt.state |= QStyle::State_Sunken;
	else
		opt.state &= ~QStyle::State_Sunken;
	opt.currentText.setNum(m_iValue);
	painter.drawComplexControl(QStyle::CC_ComboBox, opt);
	painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}


void QSliderComboBox::setValue(int i)
{
	m_iValue = i;
	repaint();
	emit valueChanged(i);
}

void QSliderComboBox::mousePressEvent(QMouseEvent *e)
{
	if( m_pSlider->isVisible())
	{
		m_pSlider->hide();
	} else
	{
		QPoint pos = (layoutDirection() == Qt::RightToLeft) ? rect().bottomRight() : rect().bottomLeft();
		QPoint pos2 = (layoutDirection() == Qt::RightToLeft) ? rect().topRight() : rect().topLeft();
		pos = mapToGlobal(pos);
		pos2 = mapToGlobal(pos2);
		QSize size = m_pSlider->sizeHint();
		size.setWidth(width());
		QRect screen = QApplication::desktop()->availableGeometry(pos);
		//handle popup falling "off screen"
		if (layoutDirection() == Qt::RightToLeft)
		{
			pos.setX(pos.x()-size.width());
			pos2.setX(pos2.x()-size.width());
			if (pos.x() < screen.left())
				pos.setX(qMax(pos.x(), screen.left()));
			else if (pos.x()+size.width() > screen.right())
				pos.setX(qMax(pos.x()-size.width(), screen.right()-size.width()));
		} else
		{
			if (pos.x()+size.width() > screen.right())
				pos.setX(screen.right()-size.width());
			pos.setX(qMax(pos.x(), screen.left()));
		}
		if (pos.y() + size.height() > screen.bottom())
			pos.setY(pos2.y() - size.height());
		else if (pos.y() < screen.top())
			pos.setY(screen.top());
		if (pos.y() < screen.top())
			pos.setY(screen.top());
		if (pos.y()+size.height() > screen.bottom())
			pos.setY(screen.bottom()-size.height());
		m_qFrame->move(pos);
		m_qFrame->resize(size);
		m_pSlider->resize(size);
		m_pSlider->setValue(m_iValue);
		m_qFrame->show();
	}
	QComboBox::mousePressEvent(e);
}

void QSliderComboBox::setMinimum(int v)
{
	m_pSlider->setMinimum(v);
}

void QSliderComboBox::setMaximum(int v)
{
	m_pSlider->setMaximum(v);
}

void QSliderComboBox::hidePopup()
{
	m_qFrame->hide();
}
