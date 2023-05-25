#pragma once
#include "Shader.h"
#include "Renderer.h"
#include "Transform.h"
#include "Texture.h"
#include "Camera.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Line.h"
#include "VIBuffer_Cube.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	void Clear_LevelResources(_uint iLevelIndex);
	HRESULT	Delete_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

private:
	_uint	m_iNumLevels = { 0 };

private:
	typedef unordered_map<const _tchar*, class CComponent*>	PROTOTYPES;
	PROTOTYPES* m_pPrototypes = { nullptr };
	
private:
	class CComponent*	Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END