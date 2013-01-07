#ifndef STANDARDMOUSEACTIONS_H
#define STANDARDMOUSEACTIONS_H

class QToolBar;
class QComboBox;
class CMouseSelect;
class CMouseMove;
class CMouseRotate;
class CMouseScale;

class CStandardMouseActions
{
public:
    CStandardMouseActions();

	void InitActions();

private:
	QToolBar* m_qToolbar;
	QComboBox* m_qCombo;
	CMouseSelect *m_pMouseSelect;
	CMouseMove *m_pMouseMove;
	CMouseRotate *m_pMouseRotate;
	CMouseScale *m_pMouseScale;
};

#endif // STANDARDMOUSEACTIONS_H
