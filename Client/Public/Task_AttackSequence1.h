#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_AttackSequence1 final : public CTask
{
private:
	explicit CTask_AttackSequence1() = default;
	virtual ~CTask_AttackSequence1() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	_double*			m_dAttackSequence1Time = { nullptr };
	_bool*				m_isAttackSequence1 = { nullptr }; 
	CTransform*			m_pTransformCom = { nullptr };

public:
	static CTask_AttackSequence1* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END