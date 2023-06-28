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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPart::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CPart::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
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
