#include "Task_LookAt.h"
#include "GameObject.h"

HRESULT CTask_LookAt::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_isWait = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isWait")));
	m_isWalk = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isWalk")));
	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));

	return S_OK;
}

CBehavior::STATE CTask_LookAt::Tick(_double dTimeDelta)
{
	CGameObject* pTarget = *reinterpret_cast<CGameObject**>(m_pBlackBoard->Find_Value(TEXT("Value_Target")));

	if (nullptr == pTarget)
		return STATE_FAILED;

	*m_isWait = true;
	*m_isWalk = false;
	m_pTransformCom->LookAt(pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION), true);

	return STATE_SUCCESS;
}

HRESULT CTask_LookAt::Reset()
{
	return S_OK;
}

CTask_LookAt* CTask_LookAt::Create(CBlackBoard* pBlackBoard)
{
	CTask_LookAt* pInstance = new CTask_LookAt();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_LookAt");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_LookAt::Free()
{
	__super::Free();
}
