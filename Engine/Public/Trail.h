#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTrail : public CComponent
{
private:
	explicit CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	//virtual HRESULT Render() override;

public:
	virtual HRESULT Reset() override;

private:

public:
	static CTrail* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END