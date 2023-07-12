#include "Task_Dash.h"
#include "GameInstance.h"
#include "GameObject.h"

HRESULT CTask_Dash::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_pNavigationCom = static_cast<CNavigation*>(m_pBlackBoard->Find_Value(TEXT("Value_Navigation")));
	m_isDash = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isDash")));
	m_dDashTime = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_DashTime")));
	m_dDashSpeed = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_DashSpeed")));
	m_isBlocked = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isBlocked")));

	CELLFLAG CellFlag = CELL_MOVE;
	m_pNavigationCom->Is_Move(m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr, &CellFlag);

	m_dInitDashTime = *m_dDashTime;
	m_BlockedTime = *m_dDashTime / 3.0;

	return S_OK;
}

CBehavior::STATE CTask_Dash::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_pNavigationCom)
	{
		*m_isDash = false;
		return STATE_FAILED;
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (false == *m_isBlocked)
	{
		*m_dDashTime = m_dInitDashTime;
		m_pTransformCom->Go_Straight(dTimeDelta * (*m_dDashSpeed));
	}
	else
	{
		*m_dDashTime = m_BlockedTime;
		m_pTransformCom->Go_Backward(dTimeDelta * (*m_dDashSpeed));
	}

	CELLFLAG CellFlag = CELL_STATIC;

	if (false == m_pNavigationCom->Is_Move(m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr, &CellFlag))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		// 못움직이면 대쉬 끝냄.
		return STATE_SUCCESS;
	}

	m_dRunningAcc += dTimeDelta;

	if (m_dRunningAcc < *m_dDashTime)
	{
		*m_isDash = true;
		return STATE_RUNNING;
	}

	m_dRunningAcc = 0.0;

	*m_isDash = false;

	return STATE_SUCCESS;
}

HRESULT CTask_Dash::Reset()
{
	return S_OK;
}

CTask_Dash* CTask_Dash::Create(CBlackBoard* pBlackBoard)
{
	CTask_Dash* pInstance = new CTask_Dash();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_Dash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Dash::Free()
{
	__super::Free();
}
