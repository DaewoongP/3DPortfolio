#include "..\Public\Enemy_Pistol.h"
#include "GameInstance.h"
CEnemy_Pistol::CEnemy_Pistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CEnemy_Pistol::CEnemy_Pistol(const CEnemy_Pistol& rhs)
	: CEnemy(rhs)
{
}

HRESULT CEnemy_Pistol::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnemy_Pistol::Initialize(void* pArg)
{
	return S_OK;
}

void CEnemy_Pistol::Tick(_double dTimeDelta)
{
}

GAMEEVENT CEnemy_Pistol::Late_Tick(_double dTimeDelta)
{
	return GAME_NOEVENT;
}

void CEnemy_Pistol::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
}

void CEnemy_Pistol::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
}

void CEnemy_Pistol::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
}

HRESULT CEnemy_Pistol::Render()
{
	return S_OK;
}

HRESULT CEnemy_Pistol::Reset()
{
	return S_OK;
}

HRESULT CEnemy_Pistol::Add_Component()
{
	return S_OK;
}

HRESULT CEnemy_Pistol::Add_Parts()
{
	return S_OK;
}

CEnemy_Pistol* CEnemy_Pistol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Pistol* pInstance = new CEnemy_Pistol(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEnemy_Pistol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_Pistol::Clone(void* pArg)
{
	CEnemy_Pistol* pInstance = new CEnemy_Pistol(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEnemy_Pistol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Pistol::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
