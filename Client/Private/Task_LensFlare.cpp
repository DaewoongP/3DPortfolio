#include "../Public/Task_LensFlare.h"
#include "GameInstance.h"
#include "LensFlare.h"

HRESULT CTask_LensFlare::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pTransformCom = static_cast<CTransform*>(m_pBlackBoard->Find_Value(TEXT("Value_Transform")));
	m_pLensFlare = static_cast<CLensFlare*>(m_pBlackBoard->Find_Value(TEXT("Value_LensFlare")));
	m_LensMatrix = static_cast<_float4x4*>(m_pBlackBoard->Find_Value(TEXT("Value_LensMatrix")));

	return S_OK;
}

CBehavior::STATE CTask_LensFlare::Tick(_double dTimeDelta)
{
	if (nullptr == m_pTransformCom ||
		nullptr == m_pLensFlare)
		return STATE_FAILED;

	m_pLensFlare->Render_Effect(XMLoadFloat4x4(m_LensMatrix) * m_pTransformCom->Get_WorldMatrix());

	return STATE_SUCCESS;
}

HRESULT CTask_LensFlare::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_LensFlare* CTask_LensFlare::Create(CBlackBoard* pBlackBoard)
{
	CTask_LensFlare* pInstance = new CTask_LensFlare();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_LensFlare");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_LensFlare::Free()
{
	__super::Free();

	Safe_Release(m_pLensFlare);
}
