#ifndef COLORCOMBOBOX_H
#define COLORCOMBOBOX_H

#include <QComboBox>
#include <QAbstractItemView>

class QColorSelector;

class QColorComboBox : public QComboBox
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Antonino Perricone")

public:
	explicit QColorComboBox(QWidget *parent = 0);
	
	const QColor& color() const { return m_color; }

signals:
	void ColorChanged(const QColor & newColor);

public slots:
	void SetColor(const QColor& c);

protected:
	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent *e);
private:
	QColor m_color;
	QColorSelector* m_pPopup;
};

class QColorSelector : public QWidget
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Antonino Perricone")
public:
	explicit QColorSelector(QWidget *parent, Qt::WindowFlags f = 0);

	void SetColor(const QColor& c);
	const QColor& color() const { return m_color; }

	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	QSize sizeHint() const;

signals:
	void ColorChanged(const QColor &newColor);

private:
	void ManageMousePos(const QPoint &p);

	QPoint m_qCenter;
	float m_fRadius;
	QColor m_color;

	enum State
	{
		None,
		Inside_Rect,
		Inside_Circle
	} m_eState;
};

#endif // COLORCOMBOBOX_H
