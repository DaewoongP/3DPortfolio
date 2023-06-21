#pragma once
#include "Assimp_Defines.h"
#include "Base.h"

BEGIN(Converter)

class CModelConverter final : public CBase
{
public:
	// �� Ÿ��
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	explicit CModelConverter();
	virtual ~CModelConverter() = default;

public:
	// Ŭ���� ���� �����Լ�
	HRESULT Convert_Model(TYPE eType, const _char* pModelFilePath);

private:
	// ������� ���� �б� ���� �Ҵ��� ����� ��ü.
	const aiScene*			m_pAIScene = { nullptr };
	// m_pAIScene�� ���� ������ �Ѱ��ֱ����� �Ҵ��� ����.
	Assimp::Importer		m_Importer;

private: /* For. BoneNode */
	// ���� ���� �ε���
	_uint					m_iCurrentNodeIndex = { 0 };
	// ������ ������ ���� ���������̳�
	vector<NODE>			m_Nodes;

private: /* For. Converted Structs */
	// ��ȯ�Ͽ� ��Ƶ� ����ü. 
	// ������ Ŭ���̾�Ʈ, ���� ���� �ѱ������ ������ ����
	MODEL					m_Model;
	// AIScene->mMeshes �� ������ ������� ����
	MESH*					m_pMesh = { nullptr };
	// AIScene->mMaterials �� ������ ������� ����
	MATERIAL*				m_pMaterial = { nullptr };
	// AIScene->mAnimations �� ������ ������� ����
	ANIMATION*				m_pAnimation = { nullptr };

private:
	// �޾ƿ��� ���� ORM �ؽ�ó�� ���� ó���ϱ����� �ε������� �־� ORM �ؽ�ó�� ������ ����
	_uint					m_iORMIndex = { 0 };

private:
	HRESULT Convert_Bones(aiNode* pNode, _uint iParentIndex, _Inout_ _uint* iChildIndex, _bool isRoot = false);
	HRESULT Sort_Bones();
	HRESULT Convert_Meshes();
	HRESULT Store_Mesh(const aiMesh* pAIMesh, _Inout_ MESH* outMesh);
	HRESULT Convert_Materials(TYPE eType, const char* pModelFilePath);
	HRESULT Convert_Animations();

private:
	// ������� ���� ����ü�� �������� ���Ϸ� ���ִ� �Լ�
	HRESULT Write_File(TYPE eType, const _tchar* pFileName);
	// ORM �ؽ�ó�� ���� ��� ���� �����ϱ� ���� �Լ�
	HRESULT Check_ORMTexture(const _tchar* pFileName);

public:
	static HRESULT Convert(TYPE eType, const _char* pModelFilePath);
	virtual void Free() override;
};

END