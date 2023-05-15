#pragma once
#include "Base.h"
#include "GameInstance.h"
BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	explicit CLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();

	/* 이 레벨에서 보여주고하는 디버깅적 요소. */
	virtual void Tick(_double dTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

END