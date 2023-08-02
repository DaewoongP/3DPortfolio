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

	_vector vLensLocalPos = XMLoadFloat4(reinterpret_cast<_float4*>(m_LensMatrix->m[3]));
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	m_pLensFlare->Render_Effect(0.2, XMVector3TransformCoord(vLensLocalPos, WorldMatrix));

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
