#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Dynamic final : public CVIBuffer
{
private:
	explicit CVIBuffer_Rect_Dynamic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Rect_Dynamic(const CVIBuffer_Rect_Dynamic& rhs);
	virtual ~CVIBuffer_Rect_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Change_TexCoord(const _float& fLeft, const _float& fTop, const _float& fSizeX, const _float& fSizeY, class CTexture * pTexture, _uint iTextureIndex = 0);

public:
	static CVIBuffer_Rect_Dynamic* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END