#include "..\Public\ModelConverter.h"

CModelConverter::CModelConverter()
{
	ZEROMEM(&m_Model);
}

HRESULT CModelConverter::Convert_Model(TYPE eType, const _char* pModelFilePath)
{
	_uint		iFlag = 0;
	
	if (TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_GlobalScale | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_GlobalScale | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

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
	if (FAILED(Convert_Materials(eType, pModelFilePath)))
	{
		MSG_BOX("Failed Convert_Materials");
		return E_FAIL;
	}

	if (TYPE_ANIM == eType)
	{
		cout << "Convert Animations..." << endl;
		if (FAILED(Convert_Animations()))
		{
			MSG_BOX("Failed Convert_Animations");
			return E_FAIL;
		}
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
	cout << "=====================================" << endl;
	return S_OK;
}

HRESULT CModelConverter::Convert_Bones(aiNode* pNode, _uint iParentIndex, _Inout_ _uint* iChildIndex, _bool isRoot)
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
		Node.Parent = -1;
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
	{
		Node.Children = new _uint[pNode->mNumChildren];
		ZeroMemory(Node.Children, sizeof(_uint) * pNode->mNumChildren);
	}
		

	for (_uint i = 0; i < pNode->mNumChildren; i++)
	{
		_uint iRetIndex = { 0 };
		Convert_Bones(pNode->mChildren[i], Node.NodeIndex, &iRetIndex);
		Node.Children[i] = iRetIndex;
	}

	m_Nodes.push_back(Node);

	return S_OK;
}

HRESULT CModelConverter::Sort_Bones()
{
	sort(m_Nodes.begin(), m_Nodes.end(), [](NODE& Sour, NODE& Dest) {

		if (Sour.NodeIndex < Dest.NodeIndex)
			return true;
		else
			return false;
	});
	return S_OK;
}

HRESULT CModelConverter::Convert_Meshes()
{
	ZEROMEM(&m_Model);

	m_Model.NumNodes = (_uint)m_Nodes.size();

	m_Model.NumMeshes = m_pAIScene->mNumMeshes;

	m_pMesh = new MESH[m_pAIScene->mNumMeshes];
	ZeroMemory(m_pMesh, sizeof(MESH) * m_pAIScene->mNumMeshes);

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

		m_pMesh[i] = Mesh;
	}

	return S_OK;
}

HRESULT CModelConverter::Store_Mesh(const aiMesh* pAIMesh, _Inout_ MESH* outMesh)
{
	outMesh->MaterialIndex = pAIMesh->mMaterialIndex;

	_tchar MeshName[256] = TEXT("");
	CharToWChar(pAIMesh->mName.data, MeshName);
	lstrcpy(outMesh->Name, MeshName);

	outMesh->NumVertices = pAIMesh->mNumVertices;
	outMesh->NumFaces = pAIMesh->mNumFaces;
	outMesh->Faces = new FACE[outMesh->NumFaces];
	ZeroMemory(outMesh->Faces, sizeof(FACE) * outMesh->NumFaces);

	for (_uint i = 0; i < pAIMesh->mNumFaces; ++i)
	{
		outMesh->Faces[i].NumIndices = pAIMesh->mFaces[i].mNumIndices;

		outMesh->Faces[i].Indices = new _uint[outMesh->Faces[i].NumIndices];
		ZeroMemory(outMesh->Faces[i].Indices, sizeof(_uint) * outMesh->Faces[i].NumIndices);

		for (_uint j = 0; j < outMesh->Faces[i].NumIndices; ++j)
		{
			outMesh->Faces[i].Indices[j] = pAIMesh->mFaces[i].mIndices[j];
		}
	}

	outMesh->Positions = new _float3[pAIMesh->mNumVertices];
	ZeroMemory(outMesh->Positions, sizeof(_float3) * pAIMesh->mNumVertices);

	outMesh->Normals = new _float3[pAIMesh->mNumVertices];
	ZeroMemory(outMesh->Normals, sizeof(_float3) * pAIMesh->mNumVertices);

	outMesh->TexCoords = new _float2[pAIMesh->mNumVertices];
	ZeroMemory(outMesh->TexCoords, sizeof(_float2) * pAIMesh->mNumVertices);

	outMesh->Tangents = new _float3[pAIMesh->mNumVertices];
	ZeroMemory(outMesh->Tangents, sizeof(_float3) * pAIMesh->mNumVertices);

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
	ZeroMemory(outMesh->Bones, sizeof(BONE) * pAIMesh->mNumBones);

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
		ZeroMemory(Bone.Weights, sizeof(WEIGHT) * pAIBone->mNumWeights);

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

HRESULT CModelConverter::Convert_Materials(TYPE eType, const char* pModelFilePath)
{
	m_Model.NumMaterials = m_pAIScene->mNumMaterials;

	m_pMaterial = new MATERIAL[m_pAIScene->mNumMaterials];
	ZeroMemory(m_pMaterial, sizeof(MATERIAL) * m_pAIScene->mNumMaterials);

	m_Model.ORMTextures = new ORMTEXTURE[50];
	ZeroMemory(m_Model.ORMTextures, sizeof(ORMTEXTURE) * 50);

	for (_uint i = 0; i < m_pAIScene->mNumMaterials; ++i)
	{
		MATERIAL Material;
		ZEROMEM(&Material);

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString	aistrPath;

			if (FAILED(m_pAIScene->mMaterials[i]->GetTexture(aiTextureType(j), 0, &aistrPath)))
				continue;
			
			_char pPath[MAX_STR] = "";
			strcpy_s(pPath, MAX_STR, aistrPath.data);
			string TexturePath = pPath;

			_char szDrivePath[MAX_PATH] = "";
			_char szModelPath[MAX_PATH] = "";
			_splitpath_s(pModelFilePath, szDrivePath, MAX_PATH, szModelPath, MAX_PATH, nullptr, 0, nullptr, 0);

			string DrivePath = szDrivePath;
			string ModelDirPath = szModelPath;

			size_t iOffset = string::npos;
			while (string::npos != (iOffset = ModelDirPath.find("\\")))
			{
				ModelDirPath.replace(iOffset, 1, "/");
			}

			while (string::npos != (iOffset = TexturePath.find("\\")))
			{
				TexturePath.replace(iOffset, 1, "/");
			}
			if (eType == TYPE_ANIM)
			{
				size_t DirSlashIndexTest = ModelDirPath.rfind("/");

				ModelDirPath = ModelDirPath.substr(0, DirSlashIndexTest);

				DirSlashIndexTest = ModelDirPath.rfind("/");

				ModelDirPath = ModelDirPath.substr(0, DirSlashIndexTest + 1);
			}

			while (string::npos != TexturePath.find("../"))
			{
				size_t DirSlashIndex = ModelDirPath.rfind("/");

				ModelDirPath = ModelDirPath.substr(0, DirSlashIndex);

				DirSlashIndex = ModelDirPath.rfind("/");

				ModelDirPath = ModelDirPath.substr(0, DirSlashIndex + 1);
				//_splitpath_s(ModelDirPath.data(), nullptr, 0, ModelDirPath.data(), MAX_PATH, nullptr, 0, nullptr, 0);

				TexturePath = TexturePath.substr(3);
			}

			string FullPath = DrivePath + ModelDirPath + TexturePath;
			
			_tchar wszFullPath[MAX_PATH] = TEXT("");
			CharToWChar(FullPath.c_str(), wszFullPath);
			
			lstrcpy(Material.MaterialTexture[j].TexPath, wszFullPath);

			Material.MaterialTexture[j].TexType = TEXTYPE(j);
		}

		m_pMaterial[i] = Material;
	}

	m_Model.NumORMTextures = m_iORMIndex;
	return S_OK;
}

HRESULT CModelConverter::Convert_Animations()
{
	m_Model.NumAnimations = m_pAIScene->mNumAnimations;

	m_pAnimation = new ANIMATION[m_pAIScene->mNumAnimations];
	ZeroMemory(m_pAnimation, sizeof(ANIMATION) * m_pAIScene->mNumAnimations);

	for (_uint i = 0; i < m_pAIScene->mNumAnimations; ++i)
	{
		aiAnimation* pAIAnimation = m_pAIScene->mAnimations[i];

		ANIMATION Animation;
		ZEROMEM(&Animation);
		// name
		_tchar AnimName[256] = TEXT("");
		CharToWChar(pAIAnimation->mName.data, AnimName);
		lstrcpy(Animation.Name, AnimName);
		
		Animation.Duration = pAIAnimation->mDuration;
		Animation.TickPerSecond = pAIAnimation->mTicksPerSecond;
		Animation.NumChannels = pAIAnimation->mNumChannels;

		Animation.Channels = new CHANNEL[pAIAnimation->mNumChannels];
		ZeroMemory(Animation.Channels, sizeof(CHANNEL) * pAIAnimation->mNumChannels);

		for (_uint j = 0; j < pAIAnimation->mNumChannels; ++j)
		{
			aiNodeAnim* pAIChannel = pAIAnimation->mChannels[j];
			
			CHANNEL Channel;
			ZEROMEM(&Channel);
			// name
			_tchar ChannelName[256] = TEXT("");
			CharToWChar(pAIChannel->mNodeName.data, ChannelName);
			lstrcpy(Channel.Name, ChannelName);
			
			// Scale
			Channel.NumScalingKeys = pAIChannel->mNumScalingKeys;
			Channel.ScalingKeys = new VECTORKEY[pAIChannel->mNumScalingKeys];
			ZeroMemory(Channel.ScalingKeys, sizeof(VECTORKEY) * pAIChannel->mNumScalingKeys);

			for (_uint k = 0; k < pAIChannel->mNumScalingKeys; ++k)
			{
				Channel.ScalingKeys[k].Time = pAIChannel->mScalingKeys[k].mTime;
				memcpy(&Channel.ScalingKeys[k].Value, &pAIChannel->mScalingKeys[k].mValue, sizeof _float3);
			}

			// Rotation
			Channel.NumRotationKeys = pAIChannel->mNumRotationKeys;
			Channel.RotationKeys = new QUATERNIONKEY[pAIChannel->mNumRotationKeys];
			ZeroMemory(Channel.RotationKeys, sizeof(QUATERNIONKEY) * pAIChannel->mNumRotationKeys);

			for (_uint k = 0; k < pAIChannel->mNumRotationKeys; ++k)
			{
				Channel.RotationKeys[k].Time = pAIChannel->mRotationKeys[k].mTime;

				Channel.RotationKeys[k].Value.x = pAIChannel->mRotationKeys[k].mValue.x;
				Channel.RotationKeys[k].Value.y = pAIChannel->mRotationKeys[k].mValue.y;
				Channel.RotationKeys[k].Value.z = pAIChannel->mRotationKeys[k].mValue.z;
				Channel.RotationKeys[k].Value.w = pAIChannel->mRotationKeys[k].mValue.w;
			}

			// Position
			Channel.NumPositionKeys = pAIChannel->mNumPositionKeys;
			Channel.PositionKeys = new VECTORKEY[pAIChannel->mNumPositionKeys];
			ZeroMemory(Channel.PositionKeys, sizeof(VECTORKEY) * pAIChannel->mNumPositionKeys);

			for (_uint k = 0; k < pAIChannel->mNumPositionKeys; ++k)
			{
				Channel.PositionKeys[k].Time = pAIChannel->mPositionKeys[k].mTime;
				memcpy(&Channel.PositionKeys[k].Value, &pAIChannel->mPositionKeys[k].mValue, sizeof _float3);
			}

			Animation.Channels[j] = Channel;
		}

		m_pAnimation[i] = Animation;
	}

	return S_OK;
}

HRESULT CModelConverter::Write_File(TYPE eType, const _tchar* pFileName)
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

	// Write ORM

	// ORM NumORMTextures
	WriteFile(hFile, &(m_Model.NumORMTextures), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.NumORMTextures; ++i)
	{
		// ORM Path
		dwStrByte = sizeof(_tchar) * (lstrlen(m_Model.ORMTextures[i].Path) + 1);
		WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		WriteFile(hFile, m_Model.ORMTextures[i].Path, dwStrByte, &dwByte, nullptr);
	}

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
		WriteFile(hFile, &(Node.Parent), sizeof(_int), &dwByte, nullptr);

		// Node NumChildren
		WriteFile(hFile, &(Node.NumChildren), sizeof(_uint), &dwByte, nullptr);

		// Node Children (array)
		WriteFile(hFile, Node.Children, sizeof(_uint) * Node.NumChildren, &dwByte, nullptr);		
	}

	// Write Meshes

	// Meshes NumMeshes
	WriteFile(hFile, &(m_Model.NumMeshes), sizeof(_uint), &dwByte, nullptr);
	
	for (_uint i = 0; i < m_Model.NumMeshes; ++i)
	{
		MESH Mesh = m_pMesh[i];

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

	for (_uint i = 0; i < m_Model.NumMaterials; ++i)
	{
		MATERIAL Material = m_pMaterial[i];

		// MaterialTex
		WriteFile(hFile, Material.MaterialTexture, sizeof(MATERIALTEX) * TextureType_MAX, &dwByte, nullptr);
	}

	// Write Animations
	if (TYPE_ANIM == eType)
	{
		// Animation NumAnimations
		WriteFile(hFile, &(m_Model.NumAnimations), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < m_Model.NumAnimations; ++i)
		{
			ANIMATION Animation = m_pAnimation[i];

			// Animation Name
			dwStrByte = (_ulong)sizeof(_tchar) * (lstrlen(Animation.Name) + 1);
			WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
			WriteFile(hFile, Animation.Name, dwStrByte, &dwByte, nullptr);

			// Animation Duration
			WriteFile(hFile, &(Animation.Duration), sizeof(_double), &dwByte, nullptr);
			
			// Animation TickPerSecond
			WriteFile(hFile, &(Animation.TickPerSecond), sizeof(_double), &dwByte, nullptr);
			
			// Animation NumChannels
			WriteFile(hFile, &(Animation.NumChannels), sizeof(_uint), &dwByte, nullptr);

			for (_uint j = 0; j < Animation.NumChannels; ++j)
			{
				CHANNEL Channel = Animation.Channels[j];

				// Channel Name
				dwStrByte = (_ulong)sizeof(_tchar) * (lstrlen(Channel.Name) + 1);
				WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
				WriteFile(hFile, Channel.Name, dwStrByte, &dwByte, nullptr);

				// Channel NumScalingKeys
				WriteFile(hFile, &(Channel.NumScalingKeys), sizeof(_uint), &dwByte, nullptr);

				// Channel ScalingKeys
				WriteFile(hFile, Channel.ScalingKeys, sizeof(VECTORKEY) * (Channel.NumScalingKeys), &dwByte, nullptr);
				
				// Channel NumRotationKeys
				WriteFile(hFile, &(Channel.NumRotationKeys), sizeof(_uint), &dwByte, nullptr);

				// Channel RotationKeys
				WriteFile(hFile, Channel.RotationKeys, sizeof(QUATERNIONKEY) * (Channel.NumRotationKeys), &dwByte, nullptr);

				// Channel NumPositionKeys
				WriteFile(hFile, &(Channel.NumPositionKeys), sizeof(_uint), &dwByte, nullptr);

				// Channel PositionKeys
				WriteFile(hFile, Channel.PositionKeys, sizeof(VECTORKEY) * (Channel.NumPositionKeys), &dwByte, nullptr);
			}
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModelConverter::Check_ORMTexture(const _tchar* pFileName)
{
	_tchar		szDrive[MAX_PATH] = TEXT("");
	_tchar		szDirectory[MAX_PATH] = TEXT("");
	_tchar		szFileName[MAX_PATH] = TEXT("");
	_tchar		szExt[MAX_PATH] = TEXT("");
	_wsplitpath_s(pFileName, szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);

	fs::directory_iterator iter(fs::absolute(szDirectory));

	while (iter != fs::end(iter))
	{
		const fs::directory_entry& entry = *iter;

		// extension find
		if (nullptr != wcsstr(entry.path().filename().c_str(), TEXT("ORM")) || 
			nullptr != wcsstr(entry.path().filename().c_str(), TEXT("OcclusionRoughnessMetallic")))
		{
			_uint iFlag = 0;
			for (_uint i = 0; i < m_iORMIndex; ++i)
			{
				// same file check
				if (!lstrcmp(m_Model.ORMTextures[i].Path, entry.path().c_str()))
					++iFlag;
			}

			if (0 == iFlag)
			{
				// input texture
				lstrcpy(m_Model.ORMTextures[m_iORMIndex++].Path, entry.path().c_str());

				cout << entry.path().filename() << endl;
			}
		}
		iter++;
	}

	return S_OK;
}

HRESULT CModelConverter::Convert(TYPE eType, const _char* pModelFilePath)
{
	CModelConverter* pInstance = new CModelConverter();

	if (FAILED(pInstance->Convert_Model(eType, pModelFilePath)))
	{
		MSG_BOX("Failed to Created CModelConverter");
		Safe_Release(pInstance);
		return E_FAIL;
	}
	else
	{
		Safe_Release(pInstance);
		return S_OK;
	}
}

void CModelConverter::Free()
{
	// delete Nodes
	for (auto& iter : m_Nodes)
	{
		Safe_Delete_Array(iter.Children);
	}
	m_Nodes.clear();

	// delete Meshes
	for (_uint i = 0; i < m_Model.NumMeshes; ++i)
	{
		MESH Mesh = m_pMesh[i];

		for (_uint j = 0; j < Mesh.NumFaces; ++j)
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

	Safe_Delete_Array(m_pMesh);

	// delete Materials
	Safe_Delete_Array(m_pMaterial);

	// delete ORM Textures
	Safe_Delete_Array(m_Model.ORMTextures);

	// delete Animations
	for (_uint i = 0; i < m_Model.NumAnimations; ++i)
	{
		ANIMATION Animation = m_pAnimation[i];

		for (_uint j = 0; j < Animation.NumChannels; ++j)
		{
			Safe_Delete_Array(Animation.Channels[j].ScalingKeys);
			Safe_Delete_Array(Animation.Channels[j].RotationKeys);
			Safe_Delete_Array(Animation.Channels[j].PositionKeys);
		}

		Safe_Delete_Array(Animation.Channels);
	}
	Safe_Delete_Array(m_pAnimation);
}
