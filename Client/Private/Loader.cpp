#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Texture.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Katana.h"
#include "ForkLift.h"
#include "Prop.h"

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

	// 크리티컬 섹션 변수 초기화
	InitializeCriticalSection(&m_Critical_Section);

	// 쓰레드 시작 함수 호출
	// 3번째 인자로 시작할 함수포인터 대입.
	// 4번째 인자로 시작할 함수의 매개변수로 넣어줄 값 대입.
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("Failed Begin Loading Thread");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CLoader::Loading()
{
	if (FAILED(CoInitializeEx(nullptr, 0)))
		return E_FAIL;

	// 크리티컬 섹션 시작해서 다른 쓰레드가 이 안에 있는 동안 값을 변경하지 못하게 처리.
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

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("텍스쳐 로딩 중."));

	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Logo)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));
	

	lstrcpy(m_szLoading, TEXT("셰이더 로딩 중."));


	lstrcpy(m_szLoading, TEXT("객체 로딩 중."));

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_BackGround)");
		return E_FAIL;
	}
	
	lstrcpy(m_szLoading, TEXT("로딩 완료."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("텍스쳐 로딩 중."));

#ifdef _DEBUG
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Terrain)");
		return E_FAIL;
	}
#endif // _DEBUG

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));
	// 객체의 초기 상태행렬 값을 피벗을 통해 처리.
	_matrix		PivotMatrix = XMMatrixIdentity();

#ifdef _DEBUG
	/*For.Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 500, 500))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_VIBuffer_Terrain)");
		return E_FAIL;
	}

#endif // _DEBUG
	
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, -4.f, -0.5f);
	/* For.Prototype_Component_Model_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/ParsingData/Anim/Player.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Player)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Model_Katana */
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Katana"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/ParsingData/Anim/Katana.dat")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Katana)");
		return E_FAIL;
	}*/

	// 모델 데이터들을 경로안에서 순회하며 프로토타입 생성.
	//Ready_Prototype_Component_ModelData(CModel::TYPE_NONANIM, TEXT("..\\..\\Resources\\ParsingData\\NonAnim"), TEXT("Prototype_Component_NonAnimModel_"));
	
	lstrcpy(m_szLoading, TEXT("셰이더 로딩 중."));

#ifdef _DEBUG
	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Terrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"),
			VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Shader_Terrain)");
		return E_FAIL;
	}
#endif // _DEBUG

	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"),
			VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Shader_VtxNorTex)");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"),
			VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Shader_VtxMesh)");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"),
			VTXANIMMESH_DECL::Elements, VTXANIMMESH_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Shader_VtxAnimMesh)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("네비게이션정보 로딩 중."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/Navigation/YNavi.Navi")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Navigation)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("충돌체 로딩 중."));

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Collider_Sphere)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Collider_AABB)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Collider_OBB)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("객체 로딩 중."));

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Player)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Katana */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Katana"),
		CKatana::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Katana)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Prop */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Prop"),
		CProp::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Prop)");
		return E_FAIL;
	}
		
#ifdef _DEBUG
	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Camera_Free)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Terrain)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_ForkLift */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_ForkLift)");
		return E_FAIL;
	}
#endif // _DEBUG

	lstrcpy(m_szLoading, TEXT("로딩 완료."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Prototype_Component_ModelData(CModel::TYPE eType, const _tchar* pPath, const _tchar* pPrototypeTag)
{
	fs::directory_iterator iter(pPath);

	while (iter != fs::end(iter))
	{
		const fs::directory_entry& entry = *iter;

		if (!lstrcmp(entry.path().extension().c_str(), TEXT(".dat")))
		{
			wstring wstrProto = pPrototypeTag;
			wstring wstrFileName = entry.path().filename().c_str();

			wstrProto += wstrFileName.substr(0, wstrFileName.find(TEXT(".dat"), 0));

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, wstrProto.c_str(),
				CModel::Create(m_pDevice, m_pContext, eType, entry.path().c_str()))))
			{
				MSG_BOX("Failed Add_Prototype : (Model Data File)");
				return E_FAIL;
			}
		}

		iter++;
	}

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
	// 로딩이 끝날때까지 기다려야 하므로 infinite 옵션을 주어 로딩이 끝날때까지 쓰레드 대기.
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
