#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_double dTimeDelta);

public:
	HRESULT Bind_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, Engine::TextureType MaterialType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);

private: // Files
	Engine::MODEL				m_Model;
	vector<Engine::NODE*>		m_NodeDatas;
	vector<Engine::MESH*>		m_MeshDatas;
	vector<Engine::MATERIAL*>	m_MaterialDatas;

private: /* For.Bones */
	vector<class CBone*>		m_Bones;
public:
	typedef vector<class CBone*>	BONES;

private: /* For.Meshes */
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

private: /* For.Materials */
	_uint						m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>		m_Materials;

private:
	HRESULT Ready_File(const _tchar* pModelFilePath);
	HRESULT Ready_Bones(Engine::NODE* pNode, class CBone* pParent);
	HRESULT Ready_Meshes(TYPE eType, _fmatrix PivotMatrix);
	HRESULT Ready_Materials(const _tchar* pModelFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END