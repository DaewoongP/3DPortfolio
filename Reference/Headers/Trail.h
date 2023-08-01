#pragma once
#include "Composite.h"

BEGIN(Engine)

class ENGINE_DLL CTrail : public CComposite
{
private:
	explicit CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT Reset() override;

private:
	class CShader*					m_pShaderCom = { nullptr };
	class CTexture*					m_pTextureCom = { nullptr };
	class CTransform*				m_pTransformCom = { nullptr };
	class CVIBuffer_Rect_Trail*		m_pTrailBufferCom = { nullptr };

private:
	_float							m_fMinDistance = { 0.f };
	_float							m_fWidth = { 0.f };
	_double							m_dLifeTime = { 0.0 };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	
public:
	static CTrail* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END