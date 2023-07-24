#include "..\Public\Task_Summon.h"
#include "GameInstance.h"
#include "Enemy_Pistol.h"
#include "Enemy_Hammer.h"
#include "Enemy_Sword.h"

HRESULT CTask_Summon::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_vCenter = _float3(150.f, 0.f, 150.f);
	m_iObjectIndex = 1000;

	return S_OK;
}

CBehavior::STATE CTask_Summon::Tick(_double dTimeDelta)
{
	Summon_Pistol();
	Summon_Hammer();
	Summon_Sword();

	return STATE_SUCCESS;
}

HRESULT CTask_Summon::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTask_Summon::Summon_Pistol()
{
	CEnemy::ENEMYDESC EnemyDesc;
	ZEROMEM(&EnemyDesc);

	_tchar szObjectTag[MAX_STR] = TEXT("GameObject");
	_tchar szIndex[MAX_STR] = TEXT("");

	_itow_s(m_iObjectIndex, szIndex, 10);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(EnemyDesc.pModelPrototypeTag, TEXT("Prototype_Component_Model_Enemy_Pistol"));

	for (_uint i = 0; i < 2; ++i)
	{
		m_iObjectIndex++;
		lstrcat(szObjectTag, TEXT("_Pistol"));
		lstrcat(szObjectTag, szIndex);

		_float fRadius = _float(rand() % 101);
		_float fAngle = _float(rand() % 360);

		EnemyDesc.vScale = _float3(1.f, 1.f, 1.f);
		EnemyDesc.vPosition = _float4(fRadius * cosf(XMConvertToRadians(fAngle)) + m_vCenter.x,
			50.f,
			fRadius * sinf(XMConvertToRadians(fAngle)) + m_vCenter.z,
			1.f);

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS,
			TEXT("Prototype_GameObject_Enemy_Pistol"), TEXT("Layer_Enemy"), szObjectTag, &EnemyDesc)))
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CTask_Summon::Summon_Hammer()
{
	CEnemy::ENEMYDESC EnemyDesc;
	ZEROMEM(&EnemyDesc);

	_tchar szObjectTag[MAX_STR] = TEXT("GameObject");
	_tchar szIndex[MAX_STR] = TEXT("");

	_itow_s(m_iObjectIndex, szIndex, 10);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(EnemyDesc.pModelPrototypeTag, TEXT("Prototype_Component_Model_Enemy_Hammer"));

	for (_uint i = 0; i < 2; ++i)
	{
		m_iObjectIndex++;
		lstrcat(szObjectTag, TEXT("_Hammer"));
		lstrcat(szObjectTag, szIndex);

		_float fRadius = _float(rand() % 101);
		_float fAngle = _float(rand() % 360);

		EnemyDesc.vScale = _float3(1.f, 1.f, 1.f);
		EnemyDesc.vPosition = _float4(fRadius * cosf(XMConvertToRadians(fAngle)) + m_vCenter.x,
			50.f,
			fRadius * sinf(XMConvertToRadians(fAngle)) + m_vCenter.z,
			1.f);

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS,
			TEXT("Prototype_GameObject_Enemy_Hammer"), TEXT("Layer_Enemy"), szObjectTag, &EnemyDesc)))
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CTask_Summon::Summon_Sword()
{
	CEnemy::ENEMYDESC EnemyDesc;
	ZEROMEM(&EnemyDesc);

	_tchar szObjectTag[MAX_STR] = TEXT("GameObject");
	_tchar szIndex[MAX_STR] = TEXT("");

	_itow_s(m_iObjectIndex, szIndex, 10);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(EnemyDesc.pModelPrototypeTag, TEXT("Prototype_Component_Model_Enemy_Sword"));

	for (_uint i = 0; i < 2; ++i)
	{
		m_iObjectIndex++;
		lstrcat(szObjectTag, TEXT("_Sword"));
		lstrcat(szObjectTag, szIndex);

		_float fRadius = _float(rand() % 101);
		_float fAngle = _float(rand() % 360);

		EnemyDesc.vScale = _float3(1.f, 1.f, 1.f);
		EnemyDesc.vPosition = _float4(fRadius * cosf(XMConvertToRadians(fAngle)) + m_vCenter.x,
			50.f,
			fRadius * sinf(XMConvertToRadians(fAngle)) + m_vCenter.z,
			1.f);

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS,
			TEXT("Prototype_GameObject_Enemy_Sword"), TEXT("Layer_Enemy"), szObjectTag, &EnemyDesc)))
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CTask_Summon* CTask_Summon::Create(CBlackBoard* pBlackBoard)
{
	CTask_Summon* pInstance = new CTask_Summon();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_Summon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Summon::Free()
{
	__super::Free();
}
