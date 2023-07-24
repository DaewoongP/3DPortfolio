#include "..\Public\Task_Charge.h"
#include "GameObject.h"

HRESULT CTask_Charge::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_dChargeReadyTime = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_ChargeReadyTime")));
	m_dChargeSpeed = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_ChargeSpeed")));
	m_isCharge = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isCharge")));

	// 10이 마지노선인거같음.
	m_fStopLength = 13.f;

	return S_OK;
}

CBehavior::STATE CTask_Charge::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_dChargeReadyTime ||
		nullptr == m_isCharge)
		return STATE_FAILED;
	
	*m_isCharge = true;
	m_dRunningAcc += dTimeDelta;

	if (m_dRunningAcc > *m_dChargeReadyTime)
	{
		CGameObject* pTarget = *reinterpret_cast<CGameObject**>(m_pBlackBoard->Find_Value(TEXT("Value_Target")));

		if (nullptr == pTarget)
			return STATE_FAILED;

		_vector vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);

		_vector vTargetPos = XMVectorSetY(pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION), 0.f);
		_vector vDir = vTargetPos - vPos;

		if (m_fStopLength < XMVectorGetX(XMVector4Length(vDir)))
		{
			m_pTransformCom->MoveTo(vTargetPos, dTimeDelta * (*m_dChargeSpeed));

			return STATE_RUNNING;
		}
		else
		{
			*m_isCharge = false;
			m_dRunningAcc = 0.0;

			return STATE_SUCCESS;
		}
	}

	return STATE_RUNNING;
}

HRESULT CTask_Charge::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_Charge* CTask_Charge::Create(CBlackBoard* pBlackBoard)
{
	CTask_Charge* pInstance = new CTask_Charge();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_Charge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Charge::Free()
{
	__super::Free();
}
