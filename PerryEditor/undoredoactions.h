#ifndef UNDOMATRIXCHANGED_H
#define UNDOMATRIXCHANGED_H

#include "undoredo.h"
#include "selection.h"
#include <matrix.h>

class CUndoSelectionChanged : public IUndoRedo
{
public:
	CUndoSelectionChanged(const CSelection::List& _OldSelection,
			const CSelection::List& _NewSelection);

protected:
	virtual void Undo();
	virtual void Redo();

private:
	CSelection::List m_OldSelection;
	CSelection::List m_NewSelection;
};

class CUndoMatrixChanged : public IUndoRedo
{
public:
	CUndoMatrixChanged(
			const Matrix4f& OldMatrix,
			const Matrix4f& NewMatrix);

protected:
	virtual void Undo();
	virtual void Redo();

private:
	Matrix4f m_16fOldMatrix;
	Matrix4f m_16fNewMatrix;
};

#endif // UNDOMATRIXCHANGED_H
