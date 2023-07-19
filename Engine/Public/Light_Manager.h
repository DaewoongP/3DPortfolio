#pragma once
#include "Light.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	explicit CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const CLight::LIGHTDESC* Get_Light(_uint iIndex);

public:
	HRESULT Add_Lights(const CLight::LIGHTDESC& LightDesc);
	HRESULT Clear_Lights();
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	list<class CLight*>				m_Lights;

public:
	virtual void Free() override;
};

END