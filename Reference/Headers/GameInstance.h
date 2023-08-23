#pragma once
#include "PipeLine.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Sound_Manager.h"
#include "Component_Manager.h"
#include "Collision_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	explicit CGameInstance();
	virtual ~CGameInstance() = default;

public:
	// 엔진 프로젝트 초기화 함수
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	// 엔진 프로젝트 미리 공간 할당 함수
	HRESULT	Reserve_Engine(_uint iNumLevels);
	// 엔진 Tick 호출 함수
	void Tick_Engine(_double dTimeDelta);
	// 레벨 리소스 초기화 함수
	void Clear_LevelResources(_uint iLevelIndex);

public: /* For. Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

	HRESULT ResetRenderTargets();
	HRESULT Resize_Buffer(_uint& ResizeWidth, _uint& ResizeHeight);

public: /* For.Timer_Manager */
	HRESULT		Add_Timer(const _tchar * pTimerTag);
	void		Tick_Timer(const _tchar * pTimerTag);
	_double		Get_TimeDelta(const _tchar * pTimerTag);
	void		Set_SlowedTime(const _tchar * pTimerTag, _double dTime);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel * pNewLevel);
	_uint Get_CurrentLevelIndex() const;
	class CLevel* Get_CurrentLevel();
	HRESULT Render_Level();

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar * pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pLayerTag, const _tchar * pGameObjectTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const _tchar * pPrototypeTag, void* pArg);
	class CGameObject* Find_Prototype(const _tchar * pPrototypeTag);
	class CGameObject* Find_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pGameObjectTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag);
	class CLayer* Make_Layer(_uint iLevelIndex, const _tchar * pLayerTag);
	HRESULT Delete_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pGameObjectTag);
	HRESULT Clear_Layer(_uint iLevelIndex, const _tchar * pLayerTag);
	HRESULT Clear_DeadObjects(_uint iLevelIndex, const _tchar * pLayerTag);
	HRESULT SetUp_GameEvent(_uint iLevelIndex, const _tchar * pLayerTag, GAMEEVENT eEvent);

	class CGameObject* Get_LastGameObject();
	HRESULT Set_LastGameObject(class CGameObject* pGameObject);

public: /* For.Component_Manager*/
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void* pArg = nullptr);
	HRESULT	Delete_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag);
	unordered_map<const _tchar*, class CComponent*> Find_PrototypesBySubTag(_uint iLevelIndex, const _tchar * pSubTag);

public: /* For.Input_Device*/
	_bool		Get_DIKeyState(_ubyte ubyKeyID, CInput_Device::KEYSTATE eState = CInput_Device::KEY_PRESSING);
	_bool		Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID, CInput_Device::KEYSTATE eState = CInput_Device::KEY_PRESSING);
	_long		Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID);

public: /* For.PipeLine*/
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eTransformState, _fmatrix TransformStateMatrix);
	void Set_LightDepthMatrix(CPipeLine::D3DTRANSFORMSTATE eTransformState, _fmatrix LightStateMatrix);
	void Set_LightFar(_float fLightFar);
	_float4x4* Get_LightDepthFloat4x4(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	_matrix Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	_float4x4* Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	_matrix Get_TransformMatrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	_float4x4* Get_TransformFloat4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	_float4* Get_CamPosition();
	_float* Get_CamFar();
	_float* Get_LightFar();

public: /* For. Calculator */
	HRESULT Get_MouseRay(ID3D11DeviceContext * pContext, HWND hWnd, _fmatrix PickingWorldMatrix_Inverse, _Inout_ _float4 * vRayPos, _Inout_ _float4 * vRayDir);
	HRESULT Get_WorldMouseRay(ID3D11DeviceContext * pContext, HWND hWnd, _Inout_ _float4 * vRayPos, _Inout_ _float4 * vRayDir);
	_bool IsMouseInClient(ID3D11DeviceContext * pContext, HWND hWnd);
	_uint RandomChoose(vector<_float> Weights, _uint iChooseSize);
	_bool Timer(_double dAlarmTime, _double dTimeDelta);
	_fvector Get_RandomVectorInSphere(_float fRadius);

public: /* For. Collision_Manager */
	HRESULT Add_Collider(COLLISIONDESC::COLTYPE eCollisionType, class CCollider* pCollider);
	void ClearColliders();

public: /* For. Font_Manager */
	HRESULT Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath);
	HRESULT Render_Font(const _tchar * pFontTag, const _tchar * pText, const _float2 & Position, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, const _float2 & vOrigin = _float2(0.f, 0.f), _float fScale = 1.f);

public: /* For. Frustum */
	_bool isIn_WorldFrustum(_fvector vWorldPos, _float fRange = 0.f);
	vector<_float4> Get_CulledPositions() const;

public: /* For.Light_Manager */
	const CLight::LIGHTDESC* Get_Light(_uint iIndex);
	void Set_Light(_uint iIndex, CLight::LIGHTDESC LightDesc);
	class CLight* Add_Lights(const CLight::LIGHTDESC & LightDesc);
	HRESULT Clear_Lights();

public: /* For.Sound_Manager */
	HRESULT Add_Sounds(const _tchar * szSoundFilePath);
	HRESULT Play_Sound(const _tchar * szSoundTag, CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay = false);
	// 랜덤으로 실행할 사운드 개수와 태그 입력
	HRESULT Play_Sound(const _tchar * pSoundTag, _uint iNumSounds, CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay = false);
	HRESULT Play_BGM(const _tchar * szSoundTag, _float fVolume);
	HRESULT Stop_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Pause_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Restart_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Stop_AllSound();
	HRESULT Set_ChannelVolume(CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume);

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CCalculator*				m_pCalculator = { nullptr };
	class CCollision_Manager*		m_pCollision_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CRenderTarget_Manager*	m_pRenderTarget_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;
};

END