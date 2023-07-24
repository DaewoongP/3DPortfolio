#include "..\Public\Task_Landing.h"
#include "GameObject.h"

HRESULT CTask_Landing::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_dLandingSpeed = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_LandingSpeed")));
	m_isFly = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isFly")));

	m_vMapCenterPosition = _float4(10.f, 0.f, 10.f, 1.f);

	return S_OK;
}

CBehavior::STATE CTask_Landing::Tick(_double dTimeDelta)
{
	CGameObject* pGameObject = *reinterpret_cast<CGameObject**>(m_pBlackBoard->Find_Value(TEXT("Value_Target")));

	if (nullptr == pGameObject)
		return STATE_FAILED;

	_vector vTargetPos = pGameObject->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	// 2 * CenterPos - TargetPos => 중점기준으로 반대쪽 방향 포지션 구할 수 있음.
	_vector vOppositePos = 2.f * XMLoadFloat4(&m_vMapCenterPosition) - vTargetPos;
	vOppositePos = XMVectorSetY(vOppositePos, 0.f);

	m_pTransformCom->MoveTo(vOppositePos, dTimeDelta * (*m_dLandingSpeed));

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (0.1f > XMVectorGetY(vPos))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPos, 0.f));
		
		*m_isFly = false;

		return STATE_SUCCESS;
	}

	return STATE_RUNNING;
}

HRESULT CTask_Landing::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_Landing* CTask_Landing::Create(CBlackBoard* pBlackBoard)
{
	CTask_Landing* pInstance = new CTask_Landing();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_Landing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Landing::Free()
{
	__super::Free();
}
