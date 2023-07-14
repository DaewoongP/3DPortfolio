#include "Task_ChaseLook.h"
#include "GameObject.h"

HRESULT CTask_ChaseLook::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_isWalk = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isWalk")));
	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));

	return S_OK;
}

CBehavior::STATE CTask_ChaseLook::Tick(_double dTimeDelta)
{
	CGameObject* pTarget = *reinterpret_cast<CGameObject**>(m_pBlackBoard->Find_Value(TEXT("Value_Target")));

	if (nullptr == pTarget)
		return STATE_FAILED;

	*m_isWalk = true;

	// y값 전부 제거.
	_vector vTargetPos = XMVectorSetY(pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION), 0.f);
	_vector vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
	_vector vLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f));

	_vector vDir = XMVector3Normalize(vTargetPos - vPos);
	
	if (XMVectorGetX(XMVectorNearEqual(vLook, vDir, XMVectorSet(0.05f, 0.05f, 0.05f, 0.1f))))
	{
		*m_isWalk = false;
		// 현재 바라보는 방향이 근사치에 다다름.
		return STATE_SUCCESS;
	}

	// 오른쪽으로 회전해야함.
	if (0 < XMVectorGetY(XMVector3Cross(vLook, vDir)))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta);
	}
	else
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta * -1.0);
	}

	return STATE_RUNNING;
}

HRESULT CTask_ChaseLook::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_ChaseLook* CTask_ChaseLook::Create(CBlackBoard* pBlackBoard)
{
	CTask_ChaseLook* pInstance = new CTask_ChaseLook();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_ChaseLook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_ChaseLook::Free()
{
	__super::Free();
}
