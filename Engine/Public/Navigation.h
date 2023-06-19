#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
private:
	explicit CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationFilePath);
	virtual HRESULT Initialize(void* pArg);

#ifdef _DEBUG
public:
	virtual HRESULT Render();

private:
	HRESULT SetUp_ShaderResources();

#endif // _DEBUG
	
private:
	_uint							m_iCurrentIndex = { 0 };
	vector<class CCell*>			m_Cells;

#ifdef _DEBUG
private:
	class CShader*				m_pShader = { nullptr };
#endif // _DEBUG


public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END