#include "..\Public\Loader.h"
#include "GameInstance.h"

#include "Sky.h"
#include "Boss.h"
#include "Bomb.h"
#include "Wire.h"
#include "Mouse.h"
#include "Sword.h"
#include "Surge.h"
#include "Hammer.h"
#include "Player.h"
#include "Pistol.h"
#include "Bullet.h"
#include "Katana.h"
#include "MiniMap.h"
#include "ColProp.h"
#include "Texture.h"
#include "UI_Dash.h"
#include "UI_Hook.h"
#include "UI_Dead.h"
#include "Boss_Hp.h"
#include "MainSlot.h"
#include "Shuriken.h"
#include "Indicator.h"
#include "Crosshair.h"
#include "MiniEnemy.h"
#include "SwordTrail.h"
#include "Boss_Sword.h"
#include "BackGround.h"
#include "BlinkTarget.h"
#include "Enemy_Sword.h"
#include "Boss_Shield.h"
#include "Enemy_Pistol.h"
#include "Enemy_Hammer.h"
#include "GhostRunner_Logo.h"
#include "Black_BackGround.h"
#include "Loading_OuterCycle.h"
#include "Loading_InnerCycle.h"

#ifdef _DEBUG
#include "Terrain.h"
#include "ForkLift.h"
#include "Blue_Snow.h"
#include "Camera_Free.h"
#endif // _DEBUG

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
	case LEVELID::LEVEL_STAGE1:
		hr = Loading_For_Stage1();
		break;
	case LEVELID::LEVEL_STAGE2:
		hr = Loading_For_Stage2();
		break;
	case LEVELID::LEVEL_BOSS:
		hr = Loading_For_Boss();
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
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Main"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/MainMenu/Main%d.png"), 2))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Main)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Mouse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/Cursor/gr_pc_cursor.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Mouse)");
		return E_FAIL;
	}

	/////////////////////////////////////// 로딩 텍스처
	
	/* For.Prototype_Component_Texture_GhostRunner_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GhostRunner_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/MainMenu/Logo_1K.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_GhostRunner_Logo)");
		return E_FAIL;
	}
	
	/* For.Prototype_Component_Texture_Loading_OuterCycle*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_OuterCycle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/MainMenu/Loading/Loading_outerCircle.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Loading_OuterCycle)");
		return E_FAIL;
	}
	
	/* For.Prototype_Component_Texture_Loading_InnerCycle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_InnerCycle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/MainMenu/Loading/Loading_innerCirlcle.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Loading_InnerCycle)");
		return E_FAIL;
	}
	
	/* For.Prototype_Component_Texture_Black*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Black"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/Black.dds")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Black)");
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

	/* For.Prototype_GameObject_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mouse"),
		CMouse::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Mouse)");
		return E_FAIL;
	}

	/////////////////////////////////////// 로딩 객체
	
	/* For.Prototype_GameObject_GhostRunner_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GhostRunner_Logo"),
		CGhostRunner_Logo::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_GhostRunner_Logo)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_OuterCycle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_OuterCycle"),
		CLoading_OuterCycle::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_OuterCycleS)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_InnerCycle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InnerCycle"),
		CLoading_InnerCycle::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_InnerCycle)");
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_Black_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Black_BackGround"),
		CBlack_BackGround::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Black_BackGround)");
		return E_FAIL;
	}
	
	lstrcpy(m_szLoading, TEXT("로딩 완료."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("텍스쳐 로딩 중."));

	/* For.Prototype_Component_Texture_Crosshair */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Crosshair"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/Crosshair/crosshair.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Crosshair)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Dash */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/Crosshair/hud_dash_%d.png"), 2))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Dash)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Minimap*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Minimap"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/MiniMap/hud_popup_bg.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Minimap)");
		return E_FAIL;
	}
	
	/* For.Prototype_Component_Texture_MiniEnemy*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniEnemy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/MiniMap/hud_enemy_marker.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_MiniEnemy)");
		return E_FAIL;
	}
	
	/* For.Prototype_Component_Texture_Hook*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hook"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/GrapplingPoint/Grappling_point.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Hook)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Indicator */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Indicator"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/EnemyIndicator/hud_enemy_directional_marker.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Indicator)");
		return E_FAIL;
	}
	
	/* For.Prototype_Component_Texture_Dead */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dead"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/Dead/Dead.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Dead)");
		return E_FAIL;
	}
	
	/* For.Prototype_Component_Texture_Blink_Main */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blink_Main"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/Skills/Blink_Main.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Blink_Main)");
		return E_FAIL;
	}
	
	/* For.Prototype_Component_Texture_Surge_Main */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Surge_Main"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/Skills/Surge_Main.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Surge_Main)");
		return E_FAIL;
	}
	
	/* For.Prototype_Component_Texture_Blink_Target */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blink_Target"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/Skills/Blink_Target.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Blink_Target)");
		return E_FAIL;
	}
	
	/* For.Prototype_Component_Texture_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/TrailTest.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Trail)");
		return E_FAIL;
	}

#ifdef _DEBUG

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Snow)");
		return E_FAIL;
	}

#endif // _DEBUG

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));
	// 객체의 초기 상태행렬 값을 피벗을 통해 처리.
	_matrix		PivotMatrix = XMMatrixIdentity();

#ifdef _DEBUG

	CVIBuffer_Rect_Instance::INSTANCEDESC RectInstanceDesc;
	ZEROMEM(&RectInstanceDesc);
	RectInstanceDesc.vExtents = _float3(10.f, 4.f, 10.f);
	RectInstanceDesc.vSpeed = _uint2(2, 10);
	RectInstanceDesc.fLifeTime = 10.f;
	RectInstanceDesc.fHeight = 10.f;

	/*For.Prototype_Component_VIBuffer_Rect_Instance*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1, TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &RectInstanceDesc, 30))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_VIBuffer_Rect_Instance)");
		return E_FAIL;
	}

#endif // _DEBUG

	CVIBuffer_Point_Instance::INSTANCEDESC PointInstanceDesc;
	ZEROMEM(&PointInstanceDesc);

	/*For.Prototype_Component_VIBuffer_Point_Instance*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &PointInstanceDesc, 8))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_VIBuffer_Point_Instance)");
		return E_FAIL;
	}
	
	/*For.Prototype_Component_VIBuffer_Rect_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Trail"),
		CVIBuffer_Rect_Trail::Create(m_pDevice, m_pContext, 100))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_VIBuffer_Rect_Trail)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, -4.f, -0.5f);
	/* For.Prototype_Component_Model_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/ParsingData/Anim/Player.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Player)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_Katana */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Katana"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/Anim/Katana.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Katana)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationZ(XMConvertToRadians(-90.f));
	/* For.Prototype_Component_Model_Shuriken */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Shuriken"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/Anim/Shuriken.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Shuriken)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixScaling(1.f, 5.f, 20.f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	/* For.Prototype_Component_Model_Surge */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Surge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/Anim/Skill_Surge.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Surge)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Model_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/NonAnim/Sorted/SM_sky_01.dat")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Sky)");
		return E_FAIL;
	}

	// 겐지
	//PivotMatrix = XMMatrixScaling(1.2f, 1.2f, 1.2f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	///* For.Prototype_Component_Model_Katana */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1, TEXT("Prototype_Component_Model_Katana"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/Anim/SM_Overwatch_sword.dat"), PivotMatrix))))
	//{
	//	MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Katana)");
	//	return E_FAIL;
	//}

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_Enemy_Pistol */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Enemy_Pistol"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/ParsingData/Anim/Enemy_Pistol.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Enemy_Pistol)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
	/* For.Prototype_Component_Model_Weapon_Pistol */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Weapon_Pistol"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/NonAnim/Sorted/Weapon_Enemy_Pistol.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Weapon_Pistol)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixScaling(0.05f, 0.05f, 0.4f);
	/* For.Prototype_Component_Model_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bullet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/NonAnim/DefaultMesh/Cylinder.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Bullet)");
		return E_FAIL;
	}


	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_Enemy_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Enemy_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/ParsingData/Anim/Sword_Elite.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Enemy_Sword)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_Enemy_Hammer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Enemy_Hammer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/ParsingData/Anim/Enemy_Hammer.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Enemy_Hammer)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_Hammer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Hammer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/NonAnim/Sorted/Weapon_Enemy_Hammer.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Hammer)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(270.f));
	/* For.Prototype_Component_Model_Wire */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Wire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/Anim/Wire.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Wire)");
		return E_FAIL;
	}

	// 모델 데이터들을 경로안에서 순회하며 프로토타입 생성.
	Ready_Prototype_Component_ModelData(CModel::TYPE_NONANIM, LEVEL_STAGE1, TEXT("..\\..\\Resources\\ParsingData\\NonAnim\\Props"), TEXT("Prototype_Component_NonAnimModel_"));
	Ready_Prototype_Component_ModelData(CModel::TYPE_NONANIM, LEVEL_STAGE1, TEXT("..\\..\\Resources\\ParsingData\\NonAnim\\ColliderProps"), TEXT("Prototype_Component_NonAnimModel_"));

	lstrcpy(m_szLoading, TEXT("셰이더 로딩 중."));

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

	/* Prototype_Component_Shader_VtxORMMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxORMMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxORMMesh.hlsl"),
			VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Shader_VtxORMMesh)");
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

	/* Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"),
			VTXPOINTINSTANCE_DECL::Elements, VTXPOINTINSTANCE_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Shader_VtxPointInstance)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("네비게이션정보 로딩 중."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/Navigation/Test.Navi")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Navigation)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("충돌체 로딩 중."));

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Collider_Sphere)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Collider_AABB)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Collider_OBB)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("AI 로딩 중."));

	/* For.Prototype_Component_BehaviorTree */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		CBehaviorTree::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_BehaviorTree)");
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

	/* For.Prototype_GameObject_Shuriken */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shuriken"),
		CShuriken::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Shuriken)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Surge */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Surge"),
		CSurge::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Surge)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Enemy_Pistol */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Enemy_Pistol"),
		CEnemy_Pistol::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Enemy_Pistol)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Weapon_Pistol */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Pistol"),
		CPistol::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Weapon_Pistol)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Enemy_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Enemy_Sword"),
		CEnemy_Sword::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Enemy_Sword)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Enemy_Hammer */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Enemy_Hammer"),
		CEnemy_Hammer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Enemy_Hammer)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Weapon_Hammer */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Hammer"),
		CHammer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Weapon_Hammer)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Weapon_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Sword"),
		CSword::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Weapon_Sword)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Weapon_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Bullet"),
		CBullet::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Weapon_Bullet)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Sky)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Prop */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Prop"),
		CProp::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Prop)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_ColProp */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ColProp"),
		CColProp::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_ColProp)");
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_Crosshair */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Crosshair"),
		CCrosshair::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Crosshair)");
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_UI_Dash */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Dash"),
		CUI_Dash::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_UI_Dash)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_UI_Minimap */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Minimap"),
		CMiniMap::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_UI_Minimap)");
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_UI_MiniEnemy */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MiniEnemy"),
		CMiniEnemy::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_UI_MiniEnemy)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_UI_Hook */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Hook"),
		CUI_Hook::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_UI_Hook)");
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_Indicator */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Indicator"),
		CIndicator::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Indicator)");
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_UI_Dead*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Dead"),
		CUI_Dead::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_UI_Dead)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_MainSlot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainSlot"),
		CMainSlot::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_MainSlot)");
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_BlinkTarget*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BlinkTarget"),
		CBlinkTarget::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_BlinkTarget)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Wire */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wire"),
		CWire::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Wire)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_SwordTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwordTrail"),
		CSwordTrail::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_SwordTrail)");
		return E_FAIL;
	}

#ifdef _DEBUG

	/* For.Prototype_GameObject_Blue_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Blue_Snow"),
		CBlue_Snow::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Blue_Snow)");
		return E_FAIL;
	}
#endif // _DEBUG

	if (100 > m_iLoadingPercent)
		m_iLoadingPercent = 100;

	lstrcpy(m_szLoading, TEXT("로딩 완료."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage2()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Boss()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("텍스쳐 로딩 중."));

	/* For.Prototype_Component_Texture_Boss_Hp*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boss_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/Boss/Boss_Hp.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Boss_Hp)");
		return E_FAIL;
	}
	
	/* For.Prototype_Component_Texture_Boss_HpBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boss_HpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/HUD/GR/Boss/Boss_HpBar.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Boss_HpBar)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Boss_Emissive */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boss_Emissive"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/M_Adam_Boss_Body_01_Emit.dds")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Boss_Emissive)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Bomb_Emissive */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Bomb_Emissive"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/T_LongRifle_T01_E.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Bomb_Emissive)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Texture_Bomb_Explode */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Bomb_Explode"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Game_UI/Circle.png")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Bomb_Explode)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));
	// 객체의 초기 상태행렬 값을 피벗을 통해 처리.
	_matrix		PivotMatrix = XMMatrixIdentity();

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_Enemy_Bakunin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_Enemy_Bakunin"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/ParsingData/Anim/Enemy_Bakunin.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Enemy_Bakunin)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_Bakunin_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_Bakunin_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/Anim/Bakunin_Sword.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Bakunin_Sword)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_Bakunin_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_Bakunin_Shield"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/Anim/Bakunin_ArmShield.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Bakunin_Shield)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_Bakunin_Force_Field */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_Bakunin_Force_Field"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/Anim/Bakunin_Force_Field.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Bakunin_Force_Field)");
		return E_FAIL;
	}

	PivotMatrix = XMMatrixScaling(10.f, 10.f, 10.f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	/* For.Prototype_Component_Model_Bomb */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_Bomb"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/Anim/SM_Bomb.dat"), PivotMatrix))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Bomb)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Model_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/ParsingData/NonAnim/Sorted/SM_sky_01.dat")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Sky)");
		return E_FAIL;
	}

	Ready_Prototype_Component_ModelData(CModel::TYPE_NONANIM, LEVEL_BOSS, TEXT("..\\..\\Resources\\ParsingData\\NonAnim\\BossProps"), TEXT("Prototype_Component_NonAnimModel_"));

	lstrcpy(m_szLoading, TEXT("셰이더 로딩 중."));

	/* Prototype_Component_Shader_VtxTexInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Shader_VtxTexInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTexInstance.hlsl"),
			VTXRECTINSTANCE_DECL::Elements, VTXRECTINSTANCE_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Shader_VtxTexInstance)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("네비게이션정보 로딩 중."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/Navigation/Boss.Navi")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Navigation)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("AI 로딩 중."));

	/* For.Prototype_Component_BehaviorTree */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_BehaviorTree"),
		CBehaviorTree::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_BehaviorTree)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("객체 로딩 중."));
	// 이미 만들어놓은 프로토타입은 추가 생성할 필요없음.

	/* For.Prototype_GameObject_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss"),
		CBoss::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Boss)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Boss_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Sword"),
		CBoss_Sword::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Boss_Sword)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Boss_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Shield"),
		CBoss_Shield::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Boss_Shield)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Bomb */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bomb"),
		CBomb::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Bomb)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Boss_Hp */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Hp"),
		CBoss_Hp::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Boss_Hp)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("로딩 완료."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Prototype_Component_ModelData(CModel::TYPE eType, LEVELID eLevelID, const _tchar* pPath, const _tchar* pPrototypeTag)
{
	_matrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	fs::directory_iterator iter(pPath);

	while (iter != fs::end(iter))
	{
		const fs::directory_entry& entry = *iter;

		if (fs::is_directory(entry.path()))
		{
			Ready_Prototype_Component_ModelData(eType, eLevelID, entry.path().c_str(), pPrototypeTag);
		}

		if (!lstrcmp(entry.path().extension().c_str(), TEXT(".dat")))
		{
			wstring wstrProto = pPrototypeTag;
			wstring wstrFileName = entry.path().filename().c_str();

			wstrProto += wstrFileName.substr(0, wstrFileName.find(TEXT(".dat"), 0));

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, wstrProto.c_str(),
				CModel::Create(m_pDevice, m_pContext, eType, entry.path().c_str(), PivotMatrix))))
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
