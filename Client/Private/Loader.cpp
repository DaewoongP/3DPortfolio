#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Texture.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


_uint WINAPI Thread_Main(void* pArg)
{
	CLoader* pLoader = reinterpret_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 0;

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevel)
{
	m_eNextLevelID = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	FAILED_CHECK_RETURN(CoInitializeEx(nullptr, 0), E_FAIL);

	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = { 0 };

	switch (m_eNextLevelID)
	{
	case LEVELID::LEVEL_LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVELID::LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;
	}

	LeaveCriticalSection(&m_Critical_Section);

	FAILED_CHECK_RETURN(hr, E_FAIL);

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	lstrcpy(m_szLoading, TEXT("텍스쳐 로딩 중."));
	/* For.Prototype_Component_Texture_Logo */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(static_cast<_uint>(LEVELID::LEVEL_LOGO), TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2)), E_FAIL);

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));


	lstrcpy(m_szLoading, TEXT("셰이더 로딩 중."));


	lstrcpy(m_szLoading, TEXT("객체 로딩 중."));

	/* For.Prototype_GameObject_BackGround */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext)), E_FAIL);
	
	lstrcpy(m_szLoading, TEXT("로딩 완료."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	lstrcpy(m_szLoading, TEXT("텍스쳐 로딩 중."));


	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));
	if (FAILED(m_pGameInstance->Add_Prototype(static_cast<_uint>(LEVELID::LEVEL_GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 100, 100))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("셰이더 로딩 중."));


	lstrcpy(m_szLoading, TEXT("객체 로딩 중."));


	lstrcpy(m_szLoading, TEXT("로딩 완료."));

	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
