#pragma once
#include "Base.h"
#include "Assimp_Defines.h"

BEGIN(Converter)

class CModel final : public CBase
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	explicit CModel();
	virtual ~CModel() = default;

public:
	HRESULT Initialize(TYPE eType, const _char* pModelFilePath);

private:
	const aiScene*			m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer;

private:
	_uint					m_iCurrentNodeIndex = { 0 };
	vector<NODE>			m_Nodes;

private:
	MODEL					m_Model;

private:
	HRESULT Convert_Bones(aiNode* pNode, _uint iParentIndex, _Inout_ _uint* iChildIndex, _bool isRoot = false);
	HRESULT Convert_Meshes();
	HRESULT Store_Mesh(const aiMesh* pAIMesh, _Inout_ MESH* outMesh);
	HRESULT Convert_Materials(const char* pModelFilePath);
	HRESULT Convert_Animations();

private:
	HRESULT Write_File(const _tchar* pFileName);

public:
	static CModel* Create(TYPE eType, const _char* pModelFilePath);
	virtual void Free() override;
};

END