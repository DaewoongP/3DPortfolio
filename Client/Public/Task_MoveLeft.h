#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_MoveLeft final : public CTask
{
private:
	explicit CTask_MoveLeft() = default;
	virtual ~CTask_MoveLeft() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CTransform*			m_pTransformCom = { nullptr };
	_double*			m_dMoveTime = { nullptr };
	_bool*				m_isMoveLeft = { nullptr };

public:
	static CTask_MoveLeft* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END