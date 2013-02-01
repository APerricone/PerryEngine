#include "undoredo.h"
#include <log.h>

CUndoRedoManager* CUndoRedoManager::ms_pInstance = 0;

CUndoRedoManager::CUndoRedoManager()
{
	if( ms_pInstance )
	{
		ILog::Error("CUndoRedoManager already created\n");
	}
	ms_pInstance = this;
}

CUndoRedoManager::~CUndoRedoManager()
{
	ClearMemory();
}

void CUndoRedoManager::Undo()
{
	if( m_lpUndos.isEmpty() )  // error
		return;
	IUndoRedo* pAction = m_lpUndos.back();
	pAction->Undo();
	m_lpUndos.pop_back();
	m_lpRedos.push_back(pAction);
	emit Changed(this);
}

void CUndoRedoManager::Redo()
{
	if( m_lpRedos.isEmpty() )  // error
		return;
	IUndoRedo* pAction = m_lpRedos.back();
	pAction->Redo();
	m_lpRedos.pop_back();
	m_lpUndos.push_back(pAction);
	emit Changed(this);
}

void CUndoRedoManager::ClearMemory()
{
	ClearList(m_lpUndos);
	ClearList(m_lpRedos);
	emit Changed(this);
}

bool CUndoRedoManager::CanUndo() const
{
	return !m_lpUndos.empty();
}

bool CUndoRedoManager::CanRedo() const
{
	return !m_lpRedos.empty();
}

bool CUndoRedoManager::CanClearMemory() const
{
	return	!m_lpUndos.empty() ||
			!m_lpRedos.empty();
}

unsigned int CUndoRedoManager::GetUsedMemory() const
{
	return 0;
}

void CUndoRedoManager::AddAction(IUndoRedo* pNewAction)
{
	if( ms_pInstance == 0)
	{
		ILog::Error("CUndoRedoManager not created\n");
	}
	ms_pInstance->m_lpUndos.push_back(pNewAction);
	ms_pInstance->ClearList(ms_pInstance->m_lpRedos);
	emit ms_pInstance->Changed(ms_pInstance);
}

void CUndoRedoManager::ClearList(List& l)
{
	List::Iterator i;
	for(i=l.begin();i!=l.end();++i)
	{
		delete *i;
	}
	l.clear();
}
