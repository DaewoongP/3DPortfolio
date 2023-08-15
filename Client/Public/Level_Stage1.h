#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_Stage1 : public CLevel
{
private:
	explicit CLevel_Stage1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Stage1() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_double dTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Lights(const _tchar* pFilePath);
	HRESULT Ready_Layer_ShadowDepth(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Enemy(const _tchar* pLayerTag, const _tchar* pFilePath);
	HRESULT Ready_Layer_Props(const _tchar* pFilePath);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Trigger(const _tchar* pLayerTag);
	HRESULT Ready_UI(const _tchar* pLayerTag);

	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Debug(const _tchar* pLayerTag);

public:
	static CLevel_Stage1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END