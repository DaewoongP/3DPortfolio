#pragma once
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Input_Device.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	explicit CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	HRESULT	Reserve_Engine(_uint iNumLevels);
	void Tick_Engine(_double dTimeDelta);
	void Clear_LevelResources(_uint iLevelIndex);

public:
	// Graphic_Device
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

	HRESULT ResetRenderTargets();
	HRESULT Resize_Buffer(_uint& ResizeWidth, _uint& ResizeHeight);

public: /* For.Timer_Manager */
	HRESULT		Add_Timer(const _tchar * pTimerTag);
	void		Tick_Timer(const _tchar * pTimerTag);
	_double		Get_TimeDelta(const _tchar * pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel * pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar * pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pLayerTag, const _tchar * pGameObjectTag, void* pArg = nullptr);
	class CGameObject* Find_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pGameObjectTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag);
	class CGameObject* Get_LastGameObject();

public: /* For.Component_Manager*/
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void* pArg = nullptr);
	HRESULT	Delete_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag);
	unordered_map<const _tchar*, class CComponent*> Find_PrototypesBySubTag(_uint iLevelIndex, const _tchar * pSubTag);

public: /* For.Input_Device*/
	_byte		Get_DIKeyState(_ubyte ubyKeyID, CInput_Device::KEYSTATE eState = CInput_Device::KEY_PRESSING);
	_byte		Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID, CInput_Device::KEYSTATE eState = CInput_Device::KEY_PRESSING);
	_long		Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID);

public: /* For.PipeLine*/
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eTransformState, _fmatrix TransformStateMatrix);
	_matrix Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	_float4x4* Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	_matrix Get_TransformMatrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	_float4x4* Get_TransformFloat4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE eTransformState);
	_float4* Get_CamPosition();

public: /* For. Calculator */
	HRESULT Get_MouseRay(ID3D11DeviceContext * pContext, HWND hWnd, _fmatrix PickingWorldMatrix_Inverse, _Inout_ _float4 * vRayPos, _Inout_ _float4 * vRayDir);

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CCalculator*				m_pCalculator = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;
};

END