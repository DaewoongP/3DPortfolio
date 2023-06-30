#include "..\Public\Part.h"

CPart::CPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPart::CPart(const CGameObject& rhs)
	: CGameObject(rhs)
{
	ZEROMEM(&m_ParentMatrixDesc);
}

HRESULT CPart::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg, nullptr)))
		return E_FAIL;

	return S_OK;
}

void CPart::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

GAMEEVENT CPart::Late_Tick(_double dTimeDelta)
{
	_matrix BoneMatrix;

	/* 부모(뼈의 오프셋 * 뼈의 컴바인드 * 피벗) */
	BoneMatrix = XMLoadFloat4x4(&m_ParentMatrixDesc.OffsetMatrix) *
		XMLoadFloat4x4(m_ParentMatrixDesc.pCombindTransformationMatrix) *
		XMLoadFloat4x4(&m_ParentMatrixDesc.PivotMatrix);

	// 스케일 제거.
	BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
	BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
	BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);

	// 월드행렬 * 부모의 뼈행렬 * 부모의 월드행렬
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() *
		BoneMatrix * XMLoadFloat4x4(m_ParentMatrixDesc.pParentWorldMatrix));

	return __super::Late_Tick(dTimeDelta);
}

void CPart::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
}

void CPart::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
}

void CPart::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
}

HRESULT CPart::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPart::Free()
{
	__super::Free();
}
