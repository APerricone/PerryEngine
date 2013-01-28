#ifndef SLIDERCOMBOBOX_H
#define SLIDERCOMBOBOX_H

#include <QComboBox>

class QSlider;
class QStyleOptionComboBox;

class QSliderComboBox : public QComboBox
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Antonino Perricone")

public:
	explicit QSliderComboBox(QWidget *parent = 0);

	int value() const { return m_iValue; }

	void setMinimum(int);
	void setMaximum(int);
signals:
	void valueChanged(int newValue);

public slots:
	void setValue(int i);
	void hidePopup();

protected:
	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent *e);
private:
	int m_iValue;
	QFrame* m_qFrame;
	QSlider* m_pSlider;

};

#endif // SLIDERCOMBOBOX_H
