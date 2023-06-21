#include "..\Public\Bone.h"

CBone::CBone()
{
}

CBone::CBone(const CBone& rhs)
	: m_TransformationMatrix(rhs.m_TransformationMatrix)
	, m_CombinedTransformationMatrix(rhs.m_CombinedTransformationMatrix)
	, m_OffsetMatrix(rhs.m_OffsetMatrix)
	, m_iParentIndex(rhs.m_iParentIndex)
	, m_iIndex(rhs.m_iIndex)
{
	lstrcpy(m_szName, rhs.m_szName);
}

HRESULT CBone::Initialize(Engine::NODE* pNode)
{
	lstrcpy(m_szName, pNode->Name);
	memcpy(&m_TransformationMatrix, &pNode->Transformation, sizeof _float4x4);
	XMStoreFloat4x4(&m_TransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	m_iParentIndex = pNode->Parent;
	m_iIndex = pNode->NodeIndex;
	
	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const CModel::BONES& Bones)
{
	if (-1 == m_iParentIndex)
	{
		// Root�ϰ�� ������İ� �� ��ü����� ����.
		m_CombinedTransformationMatrix = m_TransformationMatrix;
	}
	else
	{
		// �θ���İ� �ڽŻ��� ����� �����Ͽ� ���û�����ķ� ó��.
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));
	}
}

CBone* CBone::Create(Engine::NODE* pNode)
{
	CBone* pInstance = new CBone();
	if (FAILED(pInstance->Initialize(pNode)))
	{
		MSG_BOX("Failed to Created CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
}
