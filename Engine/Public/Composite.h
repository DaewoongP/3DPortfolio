#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CComposite : public CComponent
{
protected:
	explicit CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CComposite(const CComposite& rhs);
	virtual ~CComposite() = default;

public:
	virtual void Tick(_double dTimeDelta);
	virtual void Late_Tick(_double dTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);
	HRESULT	Delete_Component(const _tchar* pComponentTag);

protected:
	unordered_map<const _tchar*, class CComponent*>		m_Components;

public:
	virtual void Free() override;
};

END