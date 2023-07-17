#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		tagNavigationDesc()
			: iCurrentIndex(-1) { }
		tagNavigationDesc(_int _iCurrentIndex)
			: iCurrentIndex{ _iCurrentIndex } {	}
		_int	iCurrentIndex = { -1 };

	}NAVIGATIONDESC;

public:
	_float	Get_CurrentCellY(_fvector vPosition) const;

private:
	explicit CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Reset() override;

public:
	// 네비게이션 메쉬 상에서 움직일 수 있는지 체크.
	_bool Is_Move(_fvector vPosition, _Inout_ _float3* pNormal, _Inout_ CELLFLAG* pFlag);
	HRESULT Find_MyCell(_fvector vPosition);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;

#endif // _DEBUG
	
private:
	NAVIGATIONDESC					m_NaviDesc;
	_int							m_iInitialIndex = { 0 };
	vector<class CCell*>			m_Cells;

#ifdef _DEBUG
private:
	class CShader*				m_pShader = { nullptr };
#endif // _DEBUG

private:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END