#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_MoveForward final : public CTask
{
private:
	explicit CTask_MoveForward() = default;
	virtual ~CTask_MoveForward() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	CTransform*			m_pTransformCom = { nullptr };
	_bool*				m_isWalk = { nullptr };

public:
	static CTask_MoveForward* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END