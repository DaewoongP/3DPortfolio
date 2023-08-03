#pragma once
#include "Task.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CPart;
class CTransform;
END

BEGIN(Client)

class CTask_Attack final : public CTask
{
private:
	explicit CTask_Attack() = default;
	virtual ~CTask_Attack() = default;

public:
	virtual HRESULT Initialize(CBlackBoard * pBlackBoard) override;
	virtual CBehavior::STATE Tick(_double dTimeDelta) override;
	virtual HRESULT Reset() override;

private:
	_double*				m_dAttackCoolTime = { nullptr };
	_bool*					m_isAttack = { nullptr };
	CPart*					m_pWeapon = { nullptr };

	class CLensFlare*		m_pLensFlare = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	_float4x4*				m_LensMatrix = { nullptr };
	_float4x4				m_OutMatrix;

public:
	static CTask_Attack* Create(class CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END