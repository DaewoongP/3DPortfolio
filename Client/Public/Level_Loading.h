#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	explicit CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVELID eNextLevelID);
	virtual void Tick(_double dTimeDelta);
	virtual HRESULT Render();

private:
	// 다음 레벨을 구성하기위한 쓰레드 생성 객체
	class CLoader*			m_pLoader = { nullptr };
	LEVELID					m_eNextLevelID = { LEVEL_END };

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

END