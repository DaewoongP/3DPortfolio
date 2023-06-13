#include "..\Public\Model.h"

CModel::CModel()
{
	ZEROMEM(&m_Model);
}

HRESULT CModel::Convert_Model(TYPE eType, const _char* pModelFilePath)
{
	_uint		iFlag = 0;

	if (TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	
	if (nullptr == m_pAIScene)
		return E_FAIL;
	cout << "Convert Bones..." << endl;
	if (FAILED(Convert_Bones(m_pAIScene->mRootNode, 0, nullptr, true)))
	{
		MSG_BOX("Failed Convert_Bones");
		return E_FAIL;
	}
	Sort_Bones();

	cout << "Convert Meshes..." << endl;
	if (FAILED(Convert_Meshes()))
	{
		MSG_BOX("Failed Convert_Meshes");
		return E_FAIL;
	}
	cout << "Convert Materials..." << endl;
	if (FAILED(Convert_Materials(pModelFilePath)))
	{
		MSG_BOX("Failed Convert_Materials");
		return E_FAIL;
	}

	_tchar szFileName[MAX_PATH] = TEXT("");
	_tchar FullPath[MAX_PATH] = TEXT("");
	CharToWChar(pModelFilePath, FullPath);
	_wsplitpath_s(FullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	cout << "Writing Files..." << endl;
	if (FAILED(Write_File(eType, szFileName)))
	{
		MSG_BOX("Failed Write_File");
		return E_FAIL;
	}
		
	cout << "Convert Success!" << endl;
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
	_float4x4 TransposeMatrix;
	memcpy(&TransposeMatrix, &pNode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TransposeMatrix)));
	memcpy(&Node.Transformation, &TransposeMatrix, sizeof _float4x4);
	
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

HRESULT CModel::Sort_Bones()
{
	sort(m_Nodes.begin(), m_Nodes.end(), [](NODE& Sour, NODE& Dest) {

		if (Sour.NodeIndex < Dest.NodeIndex)
			return true;
		else
			return false;
	});
	return S_OK;
}


HRESULT CModel::Convert_Meshes()
{
	ZEROMEM(&m_Model);

	m_Model.NumNodes = (_uint)m_Nodes.size();

	m_Model.NumMeshes = m_pAIScene->mNumMeshes;

	m_Model.Meshes = new MESH[m_pAIScene->mNumMeshes];

	for (_uint i = 0; i < m_pAIScene->mNumMeshes; i++)
	{
		// Mesh
		MESH Mesh;
		ZEROMEM(&Mesh);

		if (FAILED(Store_Mesh(m_pAIScene->mMeshes[i], &Mesh)))
		{
			MSG_BOX("Failed Store Mesh");
			return E_FAIL;
		}

		m_Model.Meshes[i] = Mesh;
	}

	return S_OK;
}

HRESULT CModel::Store_Mesh(const aiMesh* pAIMesh, _Inout_ MESH* outMesh)
{
	outMesh->MaterialIndex = pAIMesh->mMaterialIndex;

	_tchar MeshName[256] = TEXT("");
	CharToWChar(pAIMesh->mName.data, MeshName);
	lstrcpy(outMesh->Name, MeshName);

	outMesh->NumVertices = pAIMesh->mNumVertices;
	outMesh->NumFaces = pAIMesh->mNumFaces;
	outMesh->Faces = new FACE[outMesh->NumFaces];

	for (_uint i = 0; i < pAIMesh->mNumFaces; ++i)
	{
		outMesh->Faces[i].NumIndices = pAIMesh->mFaces[i].mNumIndices;

		outMesh->Faces[i].Indices = new _uint[outMesh->Faces[i].NumIndices];

		for (_uint j = 0; j < outMesh->Faces[i].NumIndices; ++j)
		{
			outMesh->Faces[i].Indices[j] = pAIMesh->mFaces[i].mIndices[j];
		}
	}

	outMesh->Positions = new _float3[pAIMesh->mNumVertices];
	outMesh->Normals = new _float3[pAIMesh->mNumVertices];
	outMesh->TexCoords = new _float2[pAIMesh->mNumVertices];
	outMesh->Tangents = new _float3[pAIMesh->mNumVertices];

	for (_uint i = 0; i < pAIMesh->mNumVertices; i++)
	{
		memcpy(&outMesh->Positions[i], &pAIMesh->mVertices[i], sizeof _float3);
		memcpy(&outMesh->Normals[i], &pAIMesh->mNormals[i], sizeof _float3);
		memcpy(&outMesh->TexCoords[i], &pAIMesh->mTextureCoords[0][i], sizeof _float2);
		memcpy(&outMesh->Tangents[i], &pAIMesh->mTangents[i], sizeof _float3);
	}


	// Mesh Bone
	outMesh->NumBones = pAIMesh->mNumBones;

	outMesh->Bones = new BONE[pAIMesh->mNumBones];

	for (_uint i = 0; i < pAIMesh->mNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		BONE	Bone;
		ZEROMEM(&Bone);
		// name
		_tchar BoneName[256] = TEXT("");
		CharToWChar(pAIBone->mName.data, BoneName);
		lstrcpy(Bone.Name, BoneName);
		// matrix
		_float4x4 TransposeMatrix;
		memcpy(&TransposeMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TransposeMatrix)));
		memcpy(&Bone.OffsetMatrix, &TransposeMatrix, sizeof _float4x4);
		
		// Mesh Bone Weight
		Bone.NumWeights = pAIBone->mNumWeights;
		
		Bone.Weights = new WEIGHT[pAIBone->mNumWeights];

		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight	AIVertexWeight = pAIBone->mWeights[j];

			WEIGHT Weight;
			ZEROMEM(&Weight);

			Weight.VertexId = AIVertexWeight.mVertexId;
			Weight.Weight = AIVertexWeight.mWeight;

			// store Mesh Bone Weight
			Bone.Weights[j] = Weight;
		}

		// store Mesh Bone
		outMesh->Bones[i] = Bone;
	}

	return S_OK;
}

HRESULT CModel::Convert_Materials(const char* pModelFilePath)
{
	m_Model.NumMaterials = m_pAIScene->mNumMaterials;

	m_Model.Materials = new MATERIAL[m_pAIScene->mNumMaterials];

	for (_uint i = 0; i < m_pAIScene->mNumMaterials; i++)
	{
		MATERIAL Material;
		ZEROMEM(&Material);

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString	strPath;

			if (FAILED(m_pAIScene->mMaterials[i]->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			char		szDrive[MAX_PATH] = "";
			char		szDirectory[MAX_PATH] = "";
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			char		szFileName[MAX_PATH] = "";
			char		szExt[MAX_PATH] = "";
			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			char		szFullPath[MAX_PATH] = "";
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		wszFullPath[MAX_PATH] = TEXT("");

			CharToWChar(szFullPath, wszFullPath);
			lstrcpy(Material.MaterialTexture[j].TexPath, wszFullPath);

			Material.MaterialTexture[j].TexType = TEXTYPE(j);
		}

		m_Model.Materials[i] = Material;
	}
	return S_OK;
}

HRESULT CModel::Convert_Animations()
{
	return S_OK;
}

HRESULT CModel::Write_File(TYPE eType, const _tchar* pFileName)
{
	_tchar szPath[MAX_PATH] = TEXT("../../Resources/ParsingData/");
	// Write Anim
	if (TYPE_ANIM == eType)
	{
		lstrcat(szPath, TEXT("Anim/"));
	}
	// Write NonAnim
	else
	{
		lstrcat(szPath, TEXT("NonAnim/"));
	}

	lstrcat(szPath, pFileName);
	lstrcat(szPath, TEXT(".dat"));

	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;

	// Write Nodes

	// Nodes NumNodes
	WriteFile(hFile, &(m_Model.NumNodes), sizeof(_uint), &dwByte, nullptr);

	for (auto& Node : m_Nodes)
	{
		// Node Name
		dwStrByte = sizeof(_tchar) * (lstrlen(Node.Name) + 1);
		WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		WriteFile(hFile, Node.Name, dwStrByte, &dwByte, nullptr);

		// Node Transformation
		WriteFile(hFile, &(Node.Transformation), sizeof(_float4x4), &dwByte, nullptr);

		// Node NodeIndex
		WriteFile(hFile, &(Node.NodeIndex), sizeof(_uint), &dwByte, nullptr);

		// Node Parent
		WriteFile(hFile, &(Node.Parent), sizeof(_uint), &dwByte, nullptr);

		// Node NumChildren
		WriteFile(hFile, &(Node.NumChildren), sizeof(_uint), &dwByte, nullptr);

		// Node Children (array)
		WriteFile(hFile, Node.Children, sizeof(_uint) * Node.NumChildren, &dwByte, nullptr);		
	}

	// Write Meshes

	// Meshes NumMeshes
	WriteFile(hFile, &(m_Model.NumMeshes), sizeof(_uint), &dwByte, nullptr);
	
	for (_uint i = 0; i < m_Model.NumMeshes; i++)
	{
		MESH Mesh = m_Model.Meshes[i];

		// Mesh Name
		dwStrByte = sizeof(_tchar) * (lstrlen(Mesh.Name) + 1);
		WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		WriteFile(hFile, Mesh.Name, dwStrByte, &dwByte, nullptr);

		// Mesh MaterialIndex
		WriteFile(hFile, &(Mesh.MaterialIndex), sizeof(_uint), &dwByte, nullptr);

		// Mesh NumVertices
		WriteFile(hFile, &(Mesh.NumVertices), sizeof(_uint), &dwByte, nullptr);
		
		// Mesh NumFaces
		WriteFile(hFile, &(Mesh.NumFaces), sizeof(_uint), &dwByte, nullptr);

		for (_uint j = 0; j < Mesh.NumFaces; ++j)
		{
			FACE Face = Mesh.Faces[j];
			// Face NumIndices
			WriteFile(hFile, &(Face.NumIndices), sizeof(_uint), &dwByte, nullptr);

			// Face Indices
			WriteFile(hFile, Face.Indices, sizeof(_uint) * Face.NumIndices, &dwByte, nullptr);
		}
		
		// Mesh Positions
		WriteFile(hFile, Mesh.Positions, sizeof(_float3) * Mesh.NumVertices, &dwByte, nullptr);
		
		// Mesh Normals
		WriteFile(hFile, Mesh.Normals, sizeof(_float3) * Mesh.NumVertices, &dwByte, nullptr);
		
		// Mesh TexCoords
		WriteFile(hFile, Mesh.TexCoords, sizeof(_float2) * Mesh.NumVertices, &dwByte, nullptr);
		
		// Mesh Tangents
		WriteFile(hFile, Mesh.Tangents, sizeof(_float3) * Mesh.NumVertices, &dwByte, nullptr);
		
		// Mesh NumBones
		WriteFile(hFile, &(Mesh.NumBones), sizeof(_uint), &dwByte, nullptr);
		
		// Write Bones
		for (_uint j = 0; j < Mesh.NumBones; j++)
		{
			BONE Bone = Mesh.Bones[j];

			// Bone Name
			dwStrByte = (_ulong)sizeof(_tchar) * (lstrlen(Bone.Name) + 1);
			WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
			WriteFile(hFile, Bone.Name, dwStrByte, &dwByte, nullptr);

			// Mesh OffsetMatrix
			WriteFile(hFile, &(Bone.OffsetMatrix), sizeof(_float4x4), &dwByte, nullptr);
			
			// Mesh NumWeights
			WriteFile(hFile, &(Bone.NumWeights), sizeof(_uint), &dwByte, nullptr);
			
			// Write Weights
			for (_uint k = 0; k < Bone.NumWeights; k++)
			{
				WEIGHT Weight = Bone.Weights[k];

				// Weight VertexId
				WriteFile(hFile, &(Weight.VertexId), sizeof(_uint), &dwByte, nullptr);

				// Weight Weight
				WriteFile(hFile, &(Weight.Weight), sizeof(_float), &dwByte, nullptr);
			}
		}
	}

	// Write Materials

	// Material NumMaterials
	WriteFile(hFile, &(m_Model.NumMaterials), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.NumMaterials; i++)
	{
		MATERIAL Material = m_Model.Materials[i];

		// MaterialTex
		WriteFile(hFile, Material.MaterialTexture, sizeof(MATERIALTEX) * TextureType_MAX, &dwByte, nullptr);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel::Convert(TYPE eType, const _char* pModelFilePath)
{
	CModel* pInstance = new CModel();

	if (FAILED(pInstance->Convert_Model(eType, pModelFilePath)))
	{
		MSG_BOX("Failed to Created CModel");
		Safe_Release(pInstance);
		return E_FAIL;
	}
	else
	{
		Safe_Release(pInstance);
		return S_OK;
	}
}

void CModel::Free()
{
	for (auto& iter : m_Nodes)
	{
		Safe_Delete_Array(iter.Children);
	}
	m_Nodes.clear();

	for (_uint i = 0; i < m_Model.NumMeshes; ++i)
	{
		MESH Mesh = m_Model.Meshes[i];

		for (_uint j = 0; j < Mesh.NumFaces; j++)
		{
			Safe_Delete_Array(Mesh.Faces[j].Indices);
		}
		Safe_Delete_Array(Mesh.Faces);

		Safe_Delete_Array(Mesh.Positions);
		Safe_Delete_Array(Mesh.Normals);
		Safe_Delete_Array(Mesh.TexCoords);
		Safe_Delete_Array(Mesh.Tangents);

		for (_uint j = 0; j < Mesh.NumBones; ++j)
		{
			Safe_Delete_Array(Mesh.Bones[j].Weights);
		}
		Safe_Delete_Array(Mesh.Bones);
	}
	Safe_Delete_Array(m_Model.Meshes);
	Safe_Delete_Array(m_Model.Materials);
}
