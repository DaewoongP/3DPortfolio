#include "..\Public\Bounding_AABB.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

CBounding_Sphere::CBounding_Sphere(const CBounding_Sphere& rhs)
	: CBounding(rhs)
	, m_pSphere_Original(new DirectX::BoundingSphere(*rhs.m_pSphere_Original))
	, m_pSphere(new DirectX::BoundingSphere(*m_pSphere_Original))
{
}

HRESULT CBounding_Sphere::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pSphere_Original = new DirectX::BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);

	return S_OK;
}

HRESULT CBounding_Sphere::Initialize(void* pBoundingDesc)
{
	m_pSphere_Original->Radius = static_cast<BOUNDINGSPHEREDESC*>(pBoundingDesc)->fRadius;
	m_pSphere_Original->Center = static_cast<BOUNDINGSPHEREDESC*>(pBoundingDesc)->vPosition;

	*m_pSphere = *m_pSphere_Original;

	return S_OK;
}

void CBounding_Sphere::Tick(_fmatrix WorldMatrix)
{
	// ���� ��ü���� �ش� �̵������ �޾Ƽ� ó���� ���� �ٸ� ��ü�� �Ѱ� �ְ�
	// �� �ٸ���ü�� ���� �׸�.
	m_pSphere_Original->Transform(*m_pSphere, WorldMatrix);
}

_bool CBounding_Sphere::RayIntersects(_fvector vOrigin, _fvector vDirection, _Inout_ _float& fDist)
{
	return m_pSphere->Intersects(vOrigin, vDirection, fDist);
}

_bool CBounding_Sphere::SphereIntersects(const CBounding_Sphere* pSphere)
{
	return m_pSphere->Intersects(*pSphere->m_pSphere);
}

#ifdef _DEBUG
HRESULT CBounding_Sphere::Render(_fvector vColor)
{
	// �θ��� begin�� end���� directX���� �������ִ� �������� ��ü�� ���� �׸���.
	__super::Begin();

	// ��� ��ȯ�� ���� ������ �׸���.
	DX::Draw(m_pPrimitiveBatch, *m_pSphere, vColor);

	__super::End();

	return S_OK;
}
#endif // _DEBUG

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding* CBounding_Sphere::Clone(void* pBoundingDesc)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(*this);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Cloned CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pSphere);
}
