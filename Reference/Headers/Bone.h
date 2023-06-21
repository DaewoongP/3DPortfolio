#pragma once
#include "Model.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	explicit CBone();
	explicit CBone(const CBone& rhs);
	virtual ~CBone() = default;

public:
	// 뼈의 이름 반환
	const _tchar*	Get_Name() const { return m_szName; }
	// 뼈의 최종 상태 행렬 반환.
	_float4x4		Get_CombinedTransformationMatrix() const { return m_CombinedTransformationMatrix; }
	_float4x4		Get_OffsetMatrix() const { return m_OffsetMatrix; }
	void			Set_OffsetMatrix(const _float4x4& OffsetMatrix) { m_OffsetMatrix = OffsetMatrix; }
	void			Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }

public:
	HRESULT Initialize(Engine::NODE* pNode);
	void	Invalidate_CombinedTransformationMatrix(const CModel::BONES& Bones);

private:
	_tchar			m_szName[MAX_STR] = TEXT("");
	_float4x4		m_TransformationMatrix;
	_float4x4		m_CombinedTransformationMatrix;
	_float4x4		m_OffsetMatrix;

	_int			m_iParentIndex = { -1 };
	_uint			m_iIndex = { 0 };

public:
	static CBone* Create(Engine::NODE* pNode);
	CBone* Clone();
	virtual void Free() override;
};

END