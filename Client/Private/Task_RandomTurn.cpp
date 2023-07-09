#include "..\Public\Task_RandomTurn.h"
#include "GameInstance.h"

HRESULT CTask_RandomTurn::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_dTurnTime = static_cast<_double*>(m_pBlackBoard->Find_Value(TEXT("Value_TurnTime")));
	m_isTurn = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isTurn")));
	m_isTurnLeft = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isTurnLeft")));

	if (rand() % 2)
	{
		*m_isTurnLeft = false;
		m_fRandRad = XMConvertToRadians(90.f);
	}
	else
	{
		*m_isTurnLeft = true;
		m_fRandRad = XMConvertToRadians(90.f) * -1.f;
	}

	return S_OK;
}

CBehavior::STATE CTask_RandomTurn::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_dTurnTime)
	{
		*m_isTurn = false;
		return STATE_FAILED;
	}

	m_dRunningAcc += dTimeDelta;

	if (m_dRunningAcc < *m_dTurnTime)
	{
		*m_isTurn = true;
		if (true == *m_isTurnLeft)
			m_pTransformCom->Go_Left(dTimeDelta);
		else
			m_pTransformCom->Go_Right(dTimeDelta);
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRandRad, dTimeDelta);

		return STATE_RUNNING;
	}

	// 다음로테이션 랜덤화
	if (rand() % 2)
	{
		*m_isTurnLeft = false;
		m_fRandRad = XMConvertToRadians(90.f);
	}
	else
	{
		*m_isTurnLeft = true;
		m_fRandRad = XMConvertToRadians(90.f) * -1.f;
	}

	m_dRunningAcc = 0.0;

	*m_isTurn = false;

	return STATE_SUCCESS;
}

HRESULT CTask_RandomTurn::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_RandomTurn* CTask_RandomTurn::Create(CBlackBoard* pBlackBoard)
{
	CTask_RandomTurn* pInstance = new CTask_RandomTurn();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_RandomTurn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_RandomTurn::Free()
{
	__super::Free();
}
