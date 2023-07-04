#pragma once
#include "Light.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const CLight::LIGHTDESC* Get_Light(_uint iIndex);

public:
	HRESULT Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CLight::LIGHTDESC& LightDesc);

private:
	list<class CLight*>				m_Lights;

public:
	virtual void Free() override;
};

END