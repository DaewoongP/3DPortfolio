#pragma once
#include "Level.h"
#include "Tool_Defines.h"
#include "ToolInstance.h"

BEGIN(Tool)

class CLevel_Tool final : public CLevel
{
private:
	explicit CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Tool() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_double dTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_For_Layer_Tool(const _tchar* pLayerTag);

private:
	CToolInstance*			m_pToolInstance = { nullptr };

public:
	static CLevel_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END