#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
public:
	enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

	typedef struct tagLightDesc
	{
		TYPE	eType;
		_float4	vDir;
		_float4	vPos;
		_float	fRange;

		_float4	vDiffuse;
		_float4	vAmbient;
		_float4	vSpecular;
	}LIGHTDESC;

private:
	explicit CLight() = default;
	virtual ~CLight() = default;

public:
	const LIGHTDESC* Get_LightDesc() const {
		return &m_LightDesc;
	}

public:
	HRESULT Initialize(const LIGHTDESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	LIGHTDESC					m_LightDesc;

public:
	static CLight* Create(const LIGHTDESC& LightDesc);
	virtual void Free() override;
};

END