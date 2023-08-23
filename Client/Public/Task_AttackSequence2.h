#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CTask_AttackSequence2 final : public CTask
{
private:
	explicit CTask_AttackSequence2() = default;
	virtual ~CTask_AttackSequence2() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;
	
private:
	_double*			m_dAttackSequence2Time = { nullptr };
	_bool*				m_isAttackSequence2 = { nullptr }; 
	CTransform*			m_pTransformCom = { nullptr };

public:
	static CTask_AttackSequence2* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END