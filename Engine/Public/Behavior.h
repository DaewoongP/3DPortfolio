#pragma once
#include "Base.h"
#include "BlackBoard.h"

BEGIN(Engine)

class ENGINE_DLL CBehavior abstract : public CBase
{
protected:
	enum STATE { STATE_SUCCESS, STATE_FAILED, STATE_RUNNING, STATE_END };

protected:
	explicit CBehavior();
	virtual ~CBehavior() = default;

public:
	virtual HRESULT Add_Child(CBehavior* pChild);
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard);
	virtual STATE Tick(_double dTimeDelta) PURE;

protected:
	CBlackBoard*			m_pBlackBoard;
	list<CBehavior*>		m_Childs;

public:
	virtual void Free() override;
};

END