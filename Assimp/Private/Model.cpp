#include "..\Public\Model.h"

CModel::CModel()
{
}

HRESULT CModel::Initialize(TYPE eType, const _char* pModelFilePath)
{
	_uint		iFlag = 0;

	if (TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	
	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Convert_Bones(m_pAIScene->mRootNode, 0, nullptr, true)))
		return E_FAIL;

	if (FAILED(Convert_Meshes(eType)))
		return E_FAIL;

	if (FAILED(Convert_Materials(pModelFilePath)))
		return E_FAIL;


	return S_OK;
}

HRESULT CModel::Convert_Bones(aiNode* pNode, _uint iParentIndex, _Inout_ _uint* iChildIndex, _bool isRoot)
{
	NODE Node;
	ZEROMEM(&Node);

	/// For.Bone
	// name
	_tchar BoneName[256] = TEXT("");
	CharToWChar(pNode->mName.data, BoneName);
	lstrcpy(Node.Name, BoneName);
	// matrix
	memcpy(&Node.Transformation, &pNode->mTransformation, sizeof MATRIX4X4);
	
	/// For.Node
	// root
	if (true == isRoot)
	{
		// Parent except
		Node.Parent = 0;
	}
	// child
	else
	{
		*iChildIndex = ++m_iCurrentNodeIndex;

		Node.Parent = iParentIndex;
	}

	Node.NodeIndex = m_iCurrentNodeIndex;
	Node.NumChildren = pNode->mNumChildren;

	if (0 != pNode->mNumChildren)
		Node.Children = new _uint[pNode->mNumChildren];

	for (_uint i = 0; i < pNode->mNumChildren; i++)
	{
		_uint iRetIndex = { 0 };
		Convert_Bones(pNode->mChildren[i], Node.NodeIndex, &iRetIndex);
		Node.Children[i] = iRetIndex;
	}

	m_Nodes.push_back(Node);

	return S_OK;
}


HRESULT CModel::Convert_Meshes(TYPE eType)
{
	return S_OK;
}

HRESULT CModel::Convert_Materials(const char* pModelFilePath)
{
	return S_OK;
}

HRESULT CModel::Convert_Animations()
{
	return S_OK;
}

CModel* CModel::Create(TYPE eType, const _char* pModelFilePath)
{
	CModel* pInstance = new CModel();

	if (FAILED(pInstance->Initialize(eType, pModelFilePath)))
	{
		MSG_BOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	for (auto& iter : m_Nodes)
	{
		Safe_Delete_Array(iter.Children);
	}
	m_Nodes.clear();
}
