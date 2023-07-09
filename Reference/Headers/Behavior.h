#pragma once
#include "Base.h"
#include "BlackBoard.h"
#include "Decorator.h"

BEGIN(Engine)

class ENGINE_DLL CBehavior abstract : public CBase
{
protected:
	enum STATE { STATE_SUCCESS, STATE_FAILED, STATE_RUNNING, STATE_END };

protected:
	explicit CBehavior();
	virtual ~CBehavior() = default;

public:
	void Set_Root(_bool isRoot) { m_isRoot = isRoot; }

public:
	HRESULT Add_Child(const _tchar* pChildTag, CBehavior* pChild);
	HRESULT Add_Decorator(CDecorator* pDecorator);
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard);
	virtual STATE Tick(_double dTimeDelta) PURE;

public:
	CBehavior* Find_Child(const _tchar* pChildTag);

protected:
	CBlackBoard*								m_pBlackBoard;
	unordered_map<const _tchar*, CBehavior*>	m_Childs;
	list<CDecorator*>							m_Decorators;
	_bool										m_isRoot = { false };
	_int										m_iPreRunningChildIndex = { -1 };

public:
	virtual void Free() override;
};

END