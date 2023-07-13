#include "..\Public\Task_TurnIdle.h"
#include "GameInstance.h"

HRESULT CTask_TurnIdle::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_isTurnLeft = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isTurnLeft")));

	return S_OK;
}

CBehavior::STATE CTask_TurnIdle::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_isTurnLeft)
		return STATE_FAILED;

	if (true == *m_isTurnLeft)
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f) * -1.f, true);
	else
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f), true);

	return STATE_SUCCESS;
}

HRESULT CTask_TurnIdle::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_TurnIdle* CTask_TurnIdle::Create(CBlackBoard* pBlackBoard)
{
	CTask_TurnIdle* pInstance = new CTask_TurnIdle();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_TurnIdle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_TurnIdle::Free()
{
	__super::Free();
}
