#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(_uint iNumLevels, const GRAPHICDESC & GraphicDesc, _Inout_ ID3D11Device * *ppDevice, _Inout_ ID3D11DeviceContext * *ppContext);
	void Tick_Engine(_double dTimeDelta);
	void Clear_LevelResources(_uint iLevelIndex);

public:
	// Graphic_Device
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel * pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar * pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pLayerTag, void* pArg = nullptr);

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;
};

END