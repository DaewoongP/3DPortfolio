#include "..\Public\Task_Attack.h"
#include "Part.h"

HRESULT CTask_Attack::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_dAttackCoolTime = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_AttackCoolTime")));
	m_isAttack = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isAttack")));
	m_pWeapon = static_cast<CPart*>(m_pBlackBoard->Find_Value(TEXT("Value_Weapon")));

	return S_OK;
}

CBehavior::STATE CTask_Attack::Tick(_double dTimeDelta)
{
	if (nullptr == m_dAttackCoolTime)
		return STATE_FAILED;
	
	m_dRunningAcc += dTimeDelta;
	
	if (m_dRunningAcc < *m_dAttackCoolTime)
		return STATE_SUCCESS;
	
	CGameObject* pGameObject = *reinterpret_cast<CGameObject**>(m_pBlackBoard->Find_Value(TEXT("Value_Target")));

	if (nullptr == pGameObject ||
		nullptr == m_pWeapon)
		return STATE_FAILED;

	*m_isAttack = true;

	_float4x4 PartWorldMatrix = m_pWeapon->Get_CombinedWorldMatrix();

	m_pWeapon->Attack(XMVectorSet(PartWorldMatrix._41, PartWorldMatrix._42, PartWorldMatrix._43, 1.f),
		pGameObject->Get_Transform()->Get_State(CTransform::STATE_POSITION));

	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
}

HRESULT CTask_Attack::Reset()
{
	return S_OK;
}

CTask_Attack* CTask_Attack::Create(CBlackBoard* pBlackBoard)
{
	CTask_Attack* pInstance = new CTask_Attack();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Attack::Free()
{
	__super::Free();
}
