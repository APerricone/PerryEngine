#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <QObject>
class IUndoRedo;


class CUndoRedoManager : public QObject
{
	Q_OBJECT
public:
	CUndoRedoManager();
	~CUndoRedoManager();

	void Undo();
	void Redo();
	void ClearMemory();

	bool CanUndo() const;
	bool CanRedo() const;
	bool CanClearMemory() const;
	unsigned int GetUsedMemory() const;

	static void AddAction(IUndoRedo* pNewAction);

signals:
	void Changed(CUndoRedoManager*);

private:
	static CUndoRedoManager* ms_pInstance;
	typedef QList<IUndoRedo*> List;
	List m_lpUndos;
	List m_lpRedos;

	void ClearList(List& l);

};

class IUndoRedo
{
	friend class CUndoRedoManager;
protected:
	virtual ~IUndoRedo() {}
	virtual void Undo() =0;
	virtual void Redo() =0;

};

#endif // UNDOREDO_H
