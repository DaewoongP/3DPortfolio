#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPart abstract : public CGameObject
{
public:
	typedef struct tagParentMatrixDesc
	{
		_float4x4			OffsetMatrix;
		_float4x4			PivotMatrix;
		const _float4x4*	pCombindTransformationMatrix;
		const _float4x4*	pParentWorldMatrix;
	}PARENTMATRIXDESC;

protected:
	explicit CPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPart(const CGameObject& rhs);
	virtual ~CPart() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_ParentMatrix(PARENTMATRIXDESC ParentDesc) PURE;
	virtual void	Tick(_double dTimeDelta) override;
	virtual GAMEEVENT	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	PARENTMATRIXDESC		m_ParentMatrixDesc;
	_float4x4				m_CombinedWorldMatrix;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END