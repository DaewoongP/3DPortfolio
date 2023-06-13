#include "..\Public\Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(Engine::NODE* pNode, CBone* pParent)
{
	lstrcpy(m_szName, pNode->Name);
	memcpy(&m_TransformationMatrix, &pNode->Transformation, sizeof _float4x4);
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	m_pParent = pParent;
	Safe_AddRef(m_pParent);
	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix()
{
	if (nullptr == m_pParent)
	{
		m_CombinedTransformationMatrix = m_TransformationMatrix;
	}
	else
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	}
}

CBone* CBone::Create(Engine::NODE* pNode, CBone* pParent)
{
	CBone* pInstance = new CBone();
	if (FAILED(pInstance->Initialize(pNode, pParent)))
	{
		MSG_BOX("Failed to Created CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBone::Free()
{
	Safe_Release(m_pParent);
}
