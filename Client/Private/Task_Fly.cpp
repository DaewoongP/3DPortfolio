#include "..\Public\Task_Fly.h"
#include "GameInstance.h"

HRESULT CTask_Fly::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_fFlyHeight = static_cast<_float*>(m_pBlackBoard->Find_Value(TEXT("Value_FlyHeight")));
	m_fFlySpeed = static_cast<_float*>(m_pBlackBoard->Find_Value(TEXT("Value_FlySpeed")));
	m_isFly = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isFly")));

	return S_OK;
}

CBehavior::STATE CTask_Fly::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_fFlyHeight ||
		nullptr == m_fFlySpeed)
		return STATE_FAILED;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (*m_fFlyHeight > XMVectorGetY(vPos))
	{
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		m_pTransformCom->Move_Direction(XMVector3Normalize(vUp), dTimeDelta * (*m_fFlySpeed));
		
		*m_isFly = true;

		return STATE_RUNNING;
	}

	return STATE_SUCCESS;
}

HRESULT CTask_Fly::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_Fly* CTask_Fly::Create(CBlackBoard* pBlackBoard)
{
	CTask_Fly* pInstance = new CTask_Fly();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_Fly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Fly::Free()
{
	__super::Free();
}
