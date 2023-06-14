#pragma once
#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	explicit CBone();
	virtual ~CBone() = default;

public:
	const _tchar* Get_Name() const { return m_szName; }
	_float4x4 Get_CombinedTransformationMatrix() const { return m_CombinedTransformationMatrix; }
	_float4x4 Get_OffsetMatrix() const { return m_OffsetMatrix; }
	void Set_OffsetMatrix(const _float4x4& OffsetMatrix) { m_OffsetMatrix = OffsetMatrix; }
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }
public:
	HRESULT Initialize(Engine::NODE* pNode, CBone* pParent);
	void Invalidate_CombinedTransformationMatrix();

private:
	_tchar			m_szName[MAX_STR] = TEXT("");
	_float4x4		m_TransformationMatrix;
	_float4x4		m_CombinedTransformationMatrix;
	_float4x4		m_OffsetMatrix;
	CBone*			m_pParent = { nullptr };

public:
	static CBone* Create(Engine::NODE* pNode, CBone* pParent);
	virtual void Free() override;
};

END