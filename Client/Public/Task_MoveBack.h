#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_MoveBack final : public CTask
{
private:
	explicit CTask_MoveBack() = default;
	virtual ~CTask_MoveBack() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CTransform*			m_pTransformCom = { nullptr };
	_double*			m_dMoveTime = { nullptr };
	_bool*				m_isMoveBack = { nullptr };

public:
	static CTask_MoveBack* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END