#include "colorcombobox.h"
#include <QStylePainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <math.h>

///////////////////////////////////////////////////////////////////////////////
// QColorComboBox
///////////////////////////////////////////////////////////////////////////////

QColorComboBox::QColorComboBox(QWidget *parent) :
	QComboBox(parent), m_color(64,64,192)
{
	m_pPopup = new QColorSelector(this, Qt::Popup);
	connect(m_pPopup,SIGNAL(ColorChanged(const QColor &)),
			SLOT(SetColor(const QColor &)));
}

void QColorComboBox::paintEvent(QPaintEvent *e)
{
	QStylePainter painter(this);
	// draw the combobox frame, focusrect and selected etc.
	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	if( m_pPopup->isVisible() )
		opt.state |= QStyle::State_Sunken;
	else
		opt.state &= ~QStyle::State_Sunken;
	painter.drawComplexControl(QStyle::CC_ComboBox, opt);
	// draw something in the edit

	if(opt.state & QStyle::State_Enabled)
	{
		QRect rect = painter.style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
		painter.setBrush(m_color);
		painter.drawRect(rect);
	}
}

void QColorComboBox::SetColor(const QColor& c)
{
	m_color = c;
	repaint();
	emit ColorChanged(m_color);
}

void QColorComboBox::mousePressEvent(QMouseEvent *e)
{
	if( m_pPopup->isVisible())
	{
		m_pPopup->hide();
	} else
	{
		QPoint pos = (layoutDirection() == Qt::RightToLeft) ? rect().bottomRight() : rect().bottomLeft();
		QPoint pos2 = (layoutDirection() == Qt::RightToLeft) ? rect().topRight() : rect().topLeft();
		pos = mapToGlobal(pos);
		pos2 = mapToGlobal(pos2);
		QSize size = m_pPopup->sizeHint();
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
		m_pPopup->move(pos);
		m_pPopup->SetColor(m_color);
		m_pPopup->show();
	}
}

///////////////////////////////////////////////////////////////////////////////
// QColorSelector
///////////////////////////////////////////////////////////////////////////////

QColorSelector::QColorSelector(QWidget *parent, Qt::WindowFlags f) :
	QWidget(parent,f)
{
}

void QColorSelector::paintEvent(QPaintEvent *e)
{
	QRect r = e->rect();
	QPainter painter(this);
	//painter.setBrush(QBrush(Qt::white));
	painter.setPen(Qt::NoPen);
	//painter.drawRect(r);
	/*painter.setPen(Qt::SolidLine);
	painter.drawArc(r,0,360*16);
	r.adjust(20,20,-20,-20);
	painter.drawArc(r,0,360*16);*/
	m_fRadius = (r.width()/2)-10;
	int n = int(ceilf(m_fRadius*3.1416f*2));
	m_qCenter = r.center();
	int h,s,l;
	m_color.getHsl(&h,&s,&l);
	QPoint p[4];
	QPointF a1(1,0),a2;
	for(int i=1;i<=n;i++)
	{
		QColor c;
		//c.setHsl(i * 360 / n,s,l);
		c.setHsl(i * 360 / n,128,128);
		painter.setBrush(c);
		float alpha = i * 2*3.1416f / n;
		a2=a1;
		a1 = QPointF(cosf(alpha),sinf(alpha));
		p[0] = m_qCenter + (a1 * (m_fRadius-20)).toPoint();
		p[1] = m_qCenter + (a1 * (m_fRadius)).toPoint();
		p[2] = m_qCenter + (a2 * (m_fRadius)).toPoint();
		p[3] = m_qCenter + (a2 * (m_fRadius-20)).toPoint();
		painter.drawConvexPolygon(p,4);
	}
	int dim = (m_fRadius-20)*1.41421f;
	n = qMin(dim,255);
	for(int y=0;y<n;y++)
		for(int x=0;x<n;x++)
		{
			QColor c;
			c.setHsl(h,x*255/n,y*255/n);
			painter.setBrush(c);
			int sx = m_qCenter.x()-dim/2 + x * dim / n;
			int sy = m_qCenter.y()+dim/2 - y * dim / n;
			int ex = m_qCenter.x()-dim/2 + (x+1) * dim / n;
			int ey = m_qCenter.y()+dim/2 - (y+1) * dim / n;
			painter.drawRect(sx,sy,ex-sx,ey-sy);
		}

	painter.setBrush(Qt::black);
	float alpha = h * 3.1416f / 180;
	a1 = QPointF(cosf(alpha),sinf(alpha));
	p[0] = m_qCenter + (a1 * (m_fRadius)).toPoint();
	float d = (5/m_fRadius);
	a1 = QPointF(cosf(alpha+d),sinf(alpha+d));
	p[1] = m_qCenter + (a1 * (m_fRadius+10)).toPoint();
	a1 = QPointF(cosf(alpha-d),sinf(alpha-d));
	p[2] = m_qCenter + (a1 * (m_fRadius+10)).toPoint();
	painter.drawConvexPolygon(p,3);

	p[0].setX( m_qCenter.x()-dim/2 + s * dim / 255 );
	p[0].setY( m_qCenter.y()+dim/2 - l * dim / 255 );

	painter.drawRect(p[0].x()-13,p[0].y()-2,10,4);
	painter.drawRect(p[0].x()+3,p[0].y()-2,10,4);
	painter.drawRect(p[0].x()-2,p[0].y()-13,4,10);
	painter.drawRect(p[0].x()-2,p[0].y()+3,4,10);
}

void QColorSelector::ManageMousePos(const QPoint& p)
{
	int dim = (m_fRadius-20)*1.41421f;
	QPoint d = (p-m_qCenter);
	if( m_eState == None )
	{
		QRect r(m_qCenter.x()-dim/2,m_qCenter.y()-dim/2,dim,dim);
		if( r.contains(p) )
			m_eState = Inside_Rect;
		float dist = sqrtf(d.x()*d.x()+d.y()*d.y());
		if( dist > (m_fRadius-20) && dist < m_fRadius)
			m_eState = Inside_Circle;
	}
	if( m_eState == Inside_Rect )
	{
		int h,s,l;
		m_color.getHsl(&h,&s,&l);
		s = qBound(0,(p.x()-m_qCenter.x()+dim/2) * 255 / dim,255);
		l = qBound(0,(p.y()-m_qCenter.y()+dim/2) * 255 / dim,255);

		m_color.setHsl(h,s,255-l);
		emit ColorChanged(m_color);
		repaint();
		return;
	}
	if( m_eState == Inside_Circle )
	{
		int h,s,l;
		m_color.getHsl(&h,&s,&l);
		h = atan2(float(d.y()),float(d.x())) * 180 / (3.1416f);
		if( h<0 ) h+=360;
		m_color.setHsl(h,s,l);
		emit ColorChanged(m_color);
		repaint();
	}
}

void QColorSelector::mousePressEvent(QMouseEvent *event)
{
	ManageMousePos(event->pos());
	QWidget::mousePressEvent(event);
}

void QColorSelector::mouseReleaseEvent(QMouseEvent *event)
{
	m_eState = None;
	QWidget::mouseReleaseEvent(event);
}

void QColorSelector::mouseMoveEvent(QMouseEvent *event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		ManageMousePos(event->pos());
	}
	QWidget::mouseMoveEvent(event);
}

QSize QColorSelector::sizeHint() const
{
	return QSize(255,255);
}

void QColorSelector::SetColor(const QColor& c)
{
	m_color = c;
	emit ColorChanged(m_color);
}

/*	lum = GetLum(c);
	hue = GetHue(c);
	sat = GetSat(c);
}

float GetLum(const QColor& c)
{
	c.getHsv();
}

static float GetHue(const QColor& c);
static float GetSat(const QColor& c);
static QColor GetColor(float lum,float hue,float sat);
*/
