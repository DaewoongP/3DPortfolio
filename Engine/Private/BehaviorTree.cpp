#include "..\Public\BehaviorTree.h"
#include "Behavior.h"

CBehaviorTree::CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CBehaviorTree::CBehaviorTree(const CBehaviorTree& rhs)
    : CComponent(rhs)
{
}

HRESULT CBehaviorTree::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBehaviorTree::Initialize(void* pArg)
{
	if (pArg == nullptr)
	{
		MSG_BOX("Failed Behavior Tree Argument is null");
		return E_FAIL;
	}

	m_pRootNode = static_cast<BEHAVIORDESC*>(pArg)->pRootBehavior;
	m_pBlackBoard = static_cast<BEHAVIORDESC*>(pArg)->pBlackBoard;
		
    return S_OK;
}

void CBehaviorTree::Tick(_double dTimeDelta)
{
	m_pRootNode->Tick(m_pBlackBoard, dTimeDelta);
}

CBehaviorTree* CBehaviorTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBehaviorTree* pInstance = new CBehaviorTree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBehaviorTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CBehaviorTree::Clone(void* pArg)
{
	CBehaviorTree* pInstance = new CBehaviorTree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBehaviorTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBehaviorTree::Free()
{
	__super::Free();
}
