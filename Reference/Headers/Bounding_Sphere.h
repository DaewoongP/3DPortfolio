#pragma once
#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct tagBoundingSphereDesc : public CBounding::tagBoundingDesc
	{
		_float		fRadius;

	}BOUNDINGSPHEREDESC;

private:
	explicit CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBounding_Sphere(const CBounding_Sphere& rhs);
	virtual ~CBounding_Sphere() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pBoundingDesc);

public:
	virtual void Tick(_fmatrix WorldMatrix) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(_fvector vColor = DirectX::Colors::Green) override;
#endif // _DEBUG

private:
	// ��� ��ȯ�� ��ø�Ǿ� ó���� �� �ֱ� ������ �ʿ��� ���� ��ü.
	DirectX::BoundingSphere* m_pSphere_Original = { nullptr };
	// ��� ��ȯ�� ���������� �ް� �������ϴ� ��ü.
	DirectX::BoundingSphere* m_pSphere = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding* Clone(void* pBoundingDesc) override;
	virtual void Free() override;
};

END