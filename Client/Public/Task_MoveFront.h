#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_MoveFront final : public CTask
{
private:
	explicit CTask_MoveFront() = default;
	virtual ~CTask_MoveFront() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CTransform*			m_pTransformCom = { nullptr };
	_double*			m_dMoveTime = { nullptr };
	_bool*				m_isMoveFront = { nullptr };

public:
	static CTask_MoveFront* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END