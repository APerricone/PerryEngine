#include "undoredoactions.h"
#include "selection.h"

CUndoSelectionChanged::CUndoSelectionChanged(
		const CSelection::List& _OldSelection,
		const CSelection::List& _NewSelection) :
			m_OldSelection(_OldSelection),
			m_NewSelection(_NewSelection)
{

}

void CUndoSelectionChanged::Undo()
{
	CSelection::Instance()->SetSelection(m_OldSelection);
}

void CUndoSelectionChanged::Redo()
{
	CSelection::Instance()->SetSelection(m_NewSelection);
}


CUndoMatrixChanged::CUndoMatrixChanged(
		const Matrix4f& OldMatrix,
		const Matrix4f& NewMatrix) :
			m_16fOldMatrix(OldMatrix),
			m_16fNewMatrix(NewMatrix)
{

}

void CUndoMatrixChanged::Undo()
{
	CSelection::Instance()->SetMatrix(m_16fOldMatrix);
}

void CUndoMatrixChanged::Redo()
{
	CSelection::Instance()->SetMatrix(m_16fNewMatrix);
}
