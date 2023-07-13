#include "..\Public\Task_RandomTurn.h"
#include "GameInstance.h"

HRESULT CTask_RandomTurn::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_pNavigationCom = static_cast<CNavigation*>(m_pBlackBoard->Find_Value(TEXT("Value_Navigation")));
	m_isWalk = static_cast<_bool*>(m_pBlackBoard->Find_Value(TEXT("Value_isWalk")));

	if (rand() % 2)
	{
		m_isTurnLeft = false;
		m_fRandRad = XMConvertToRadians(90.f);
	}
	else
	{
		m_isTurnLeft = true;
		m_fRandRad = XMConvertToRadians(90.f) * -1.f;
	}

	if (rand() % 2)
	{
		m_dRandRotAcc = 0.5;
	}
	else
	{
		m_dRandRotAcc = 1.0;
	}

	return S_OK;
}

CBehavior::STATE CTask_RandomTurn::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom)
	{
		return STATE_FAILED;
	}
	
	*m_isWalk = true;

	m_dRunningAcc += dTimeDelta;


	if (m_dRunningAcc < m_dRandRotAcc)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (true == m_isTurnLeft)
			m_pTransformCom->Go_Left(dTimeDelta);
		else
			m_pTransformCom->Go_Right(dTimeDelta);

		CELLFLAG CellFlag = CELL_STATIC;
		_float3 vNormal = _float3(0.f, 0.f, 0.f);

		if (false == m_pNavigationCom->Is_Move(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &vNormal, &CellFlag))
		{
			Move_Failed(vPos, XMLoadFloat3(&vNormal));
		}
		else
		{
			if (CELL_MOVE != CellFlag)
				Move_Failed(vPos, XMLoadFloat3(&vNormal));
		}

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRandRad, dTimeDelta);

		return STATE_RUNNING;
	}

	// 다음로테이션 랜덤화
	if (rand() % 2)
	{
		m_isTurnLeft = false;
		m_fRandRad = XMConvertToRadians(90.f);
	}
	else
	{
		m_isTurnLeft = true;
		m_fRandRad = XMConvertToRadians(90.f) * -1.f;
	}

	if (rand() % 2)
	{
		m_dRandRotAcc = 0.5;
	}
	else
	{
		m_dRandRotAcc = 1.0;
	}

	*m_isWalk = false;

	m_dRunningAcc = 0.0;

	return STATE_SUCCESS;
}

HRESULT CTask_RandomTurn::Reset()
{
	*m_isWalk = false;
	m_dRunningAcc = 0.0;

	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

void CTask_RandomTurn::Move_Failed(_fvector vOriginPos, _fvector vNormal)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOriginPos);

	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	// 우측으로 회전해야함.
	if (0 < XMVectorGetX(XMVector3Dot(vRight, vNormal)))
	{
		m_isTurnLeft = false;
		m_fRandRad = XMConvertToRadians(90.f);
	}
	else
	{
		m_isTurnLeft = true;
		m_fRandRad = XMConvertToRadians(90.f) * -1.f;
	}
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
