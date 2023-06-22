#include "..\Public\Model.h"
#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
	ZEROMEM(&m_Model);
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_Model(rhs.m_Model)
	, m_NodeDatas(rhs.m_NodeDatas)
	, m_MeshDatas(rhs.m_MeshDatas)
	, m_MaterialDatas(rhs.m_MaterialDatas)
	, m_AnimationDatas(rhs.m_AnimationDatas)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& pOriginalBone : rhs.m_Bones)
	{
		m_Bones.push_back(pOriginalBone->Clone());
	}

	for (auto& pMesh : m_Meshes)
	{
		Safe_AddRef(pMesh);
	}

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTexture)
			Safe_AddRef(pTexture);
	}

	for (auto& pOriginalAnimation : rhs.m_Animations)
	{
		m_Animations.push_back(pOriginalAnimation->Clone());
	}
}

const _tchar* CModel::Get_AnimationName() const
{
	return m_Animations[m_iCurrentAnimIndex]->Get_AnimationName();
}

_uint CModel::Get_AnimationFrames() const
{
	return m_Animations[m_iCurrentAnimIndex]->Get_AnimationFrames();
}

_uint CModel::Get_CurrentAnimationFrame() const
{
	return m_Animations[m_iCurrentAnimIndex]->Get_CurrentAnimationFrame();
}

_float4x4 CModel::Get_BoneCombinedTransformationMatrix(_uint iIndex)
{
	return m_Bones[iIndex]->Get_CombinedTransformationMatrix();
}

void CModel::Set_AnimationFrameSpeed(_uint iFrameIndex, _float fSpeed)
{
	m_Animations[m_iCurrentAnimIndex]->Set_FrameSpeed(iFrameIndex, fSpeed);
}

void CModel::Set_AnimationPause(_bool isPause)
{
	m_Animations[m_iCurrentAnimIndex]->Set_Pause(isPause);
}

void CModel::Set_CurrentKeyFrameIndex(_uint iKeyFrameIndex)
{
	return m_Animations[m_iCurrentAnimIndex]->Set_CurrentKeyFrameIndex(m_Bones, iKeyFrameIndex);
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Ready_File(eType, pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Bones(m_NodeDatas.front())))
		return E_FAIL;

	if (FAILED(Ready_Meshes(eType, PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Play_Animation(_double dTimeDelta)
{
	if (0 < m_iNumAnimations)
		m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_Bones, dTimeDelta);

	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}
}

HRESULT CModel::Find_BoneIndex(const _tchar* pBoneName, _uint* iIndex)
{
	*iIndex = 0;
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pValue) {
		if (!lstrcmp(pBoneName, pValue->Get_Name()))
			return true;
		else
		{
			++(*iIndex);
			return false;
		}
	});

	if (m_Bones.end() == iter)
	{
		MSG_BOX("Failed Find BoneIndex");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Bind_Material(CShader* pShader, const char* pConstantName, _uint iMeshIndex, Engine::TextureType MaterialType)
{
	if (iMeshIndex >= m_iNumMeshes ||
		MaterialType >= TextureType_MAX ||
		MaterialType < 0 ||
		m_iNumMaterials == 0 ||
		nullptr == m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMtrlTexture[MaterialType])
		return E_FAIL;

	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMtrlTexture[MaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const char* pConstantName, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[256];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 256);

	m_Meshes[iMeshIndex]->Get_Matrices(m_Bones, BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);

	return S_OK;
}

HRESULT CModel::Ready_File(TYPE eType, const _tchar* pModelFilePath)
{
	HANDLE hFile = CreateFile(pModelFilePath,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;

	ZEROMEM(&m_Model);

	// Write ORM

	// ORM NumORMTextures
	ReadFile(hFile, &(m_Model.NumORMTextures), sizeof(_uint), &dwByte, nullptr);

	m_Model.ORMTextures = new ORMTEXTURE[m_Model.NumORMTextures];
	ZeroMemory(m_Model.ORMTextures, sizeof(ORMTEXTURE) * (m_Model.NumORMTextures));

	for (_uint i = 0; i < m_Model.NumORMTextures; ++i)
	{
		// Node Name
		ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		ReadFile(hFile, m_Model.ORMTextures[i].Path, dwStrByte, &dwByte, nullptr);
		if (0 == dwByte)
		{
			MSG_BOX("Failed Read String Data");
			return E_FAIL;
		}
	}

	// Nodes NumNodes
	ReadFile(hFile, &(m_Model.NumNodes), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.NumNodes; ++i)
	{
		NODE* pNode = new NODE;

		// Node Name
		ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		ReadFile(hFile, pNode->Name, dwStrByte, &dwByte, nullptr);
		if (0 == dwByte)
		{
			MSG_BOX("Failed Read String Data");
			return E_FAIL;
		}

		// Node Transformation
		ReadFile(hFile, &(pNode->Transformation), sizeof(_float4x4), &dwByte, nullptr);

		// Node NodeIndex
		ReadFile(hFile, &(pNode->NodeIndex), sizeof(_uint), &dwByte, nullptr);

		// Node Parent
		ReadFile(hFile, &(pNode->Parent), sizeof(_int), &dwByte, nullptr);

		// Node NumChildren
		ReadFile(hFile, &(pNode->NumChildren), sizeof(_uint), &dwByte, nullptr);

		// Node Children (array)
		pNode->Children = new _uint[pNode->NumChildren];
		ZeroMemory(pNode->Children, sizeof(_uint) * (pNode->NumChildren));
		ReadFile(hFile, pNode->Children, sizeof(_uint) * (pNode->NumChildren), &dwByte, nullptr);

		m_NodeDatas.push_back(pNode);
	}

	// Write Meshes

	// Meshes NumMeshes
	ReadFile(hFile, &(m_Model.NumMeshes), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.NumMeshes; ++i)
	{
		MESH* pMesh = new MESH;
		
		// Mesh Name
		ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
		ReadFile(hFile, pMesh->Name, dwStrByte, &dwByte, nullptr);
		if (0 == dwByte)
		{
			MSG_BOX("Failed Read String Data");
			return E_FAIL;
		}

		// Mesh MaterialIndex
		ReadFile(hFile, &(pMesh->MaterialIndex), sizeof(_uint), &dwByte, nullptr);

		// Mesh NumVertices
		ReadFile(hFile, &(pMesh->NumVertices), sizeof(_uint), &dwByte, nullptr);

		// Mesh NumFaces
		ReadFile(hFile, &(pMesh->NumFaces), sizeof(_uint), &dwByte, nullptr);

		pMesh->Faces = new FACE[pMesh->NumFaces];
		ZeroMemory(pMesh->Faces, sizeof(FACE) * (pMesh->NumFaces));

		for (_uint j = 0; j < pMesh->NumFaces; ++j)
		{
			FACE Face;
			ZEROMEM(&Face);

			// Face NumIndices
			ReadFile(hFile, &(Face.NumIndices), sizeof(_uint), &dwByte, nullptr);

			// Face Indices
			Face.Indices = new _uint[Face.NumIndices];
			ZeroMemory(Face.Indices, sizeof(_uint) * (Face.NumIndices));
			ReadFile(hFile, Face.Indices, sizeof(_uint) * (Face.NumIndices), &dwByte, nullptr);
			
			pMesh->Faces[j] = Face;
		}

		// Mesh Positions
		pMesh->Positions = new _float3[pMesh->NumVertices];
		ZeroMemory(pMesh->Positions, sizeof(_float3) * (pMesh->NumVertices));
		ReadFile(hFile, pMesh->Positions, sizeof(_float3) * (pMesh->NumVertices), &dwByte, nullptr);

		// Mesh Normals
		pMesh->Normals = new _float3[pMesh->NumVertices];
		ZeroMemory(pMesh->Normals, sizeof(_float3) * (pMesh->NumVertices));
		ReadFile(hFile, pMesh->Normals, sizeof(_float3) * (pMesh->NumVertices), &dwByte, nullptr);

		// Mesh TexCoords
		pMesh->TexCoords = new _float2[pMesh->NumVertices];
		ZeroMemory(pMesh->TexCoords, sizeof(_float2) * (pMesh->NumVertices));
		ReadFile(hFile, pMesh->TexCoords, sizeof(_float2) * (pMesh->NumVertices), &dwByte, nullptr);

		// Mesh Tangents
		pMesh->Tangents = new _float3[pMesh->NumVertices];
		ZeroMemory(pMesh->Tangents, sizeof(_float3) * (pMesh->NumVertices));
		ReadFile(hFile, pMesh->Tangents, sizeof(_float3) * (pMesh->NumVertices), &dwByte, nullptr);

		// Mesh NumBones
		ReadFile(hFile, &(pMesh->NumBones), sizeof(_uint), &dwByte, nullptr);

		pMesh->Bones = new BONE[pMesh->NumBones];
		ZeroMemory(pMesh->Bones, sizeof(BONE) * (pMesh->NumBones));

		// Write Bones
		for (_uint j = 0; j < pMesh->NumBones; ++j)
		{
			BONE Bone;
			ZEROMEM(&Bone);

			// Mesh Name
			ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
			ReadFile(hFile, Bone.Name, dwStrByte, &dwByte, nullptr);
			if (0 == dwByte)
			{
				MSG_BOX("Failed Read String Data");
				return E_FAIL;
			}

			// Mesh OffsetMatrix
			ReadFile(hFile, &(Bone.OffsetMatrix), sizeof(_float4x4), &dwByte, nullptr);

			// Mesh NumWeights
			ReadFile(hFile, &(Bone.NumWeights), sizeof(_uint), &dwByte, nullptr);

			Bone.Weights = new WEIGHT[Bone.NumWeights];
			ZeroMemory(Bone.Weights, sizeof(WEIGHT)* (Bone.NumWeights));

			// Write Weights
			for (_uint k = 0; k < Bone.NumWeights; ++k)
			{
				WEIGHT Weight;
				ZEROMEM(&Weight);

				// Weight VertexId
				ReadFile(hFile, &(Weight.VertexId), sizeof(_uint), &dwByte, nullptr);

				// Weight Weight
				ReadFile(hFile, &(Weight.Weight), sizeof(_float), &dwByte, nullptr);

				Bone.Weights[k] = Weight;
			}

			pMesh->Bones[j] = Bone;
		}

		m_MeshDatas.push_back(pMesh);
	}

	// Read Materials

	// Material NumMaterials
	ReadFile(hFile, &(m_Model.NumMaterials), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.NumMaterials; ++i)
	{
		MATERIAL* pMaterial = new MATERIAL;

		// MaterialTex
		ReadFile(hFile, pMaterial->MaterialTexture, sizeof(MATERIALTEX) * TextureType_MAX, &dwByte, nullptr);

		m_MaterialDatas.push_back(pMaterial);
	}

	// Read Animations
	if (TYPE_ANIM == eType)
	{
		// Animation NumAnimations
		ReadFile(hFile, &(m_Model.NumAnimations), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < m_Model.NumAnimations; ++i)
		{
			ANIMATION* pAnimation = new ANIMATION;

			// Animation Name
			ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
			ReadFile(hFile, pAnimation->Name, dwStrByte, &dwByte, nullptr);
			if (0 == dwByte)
			{
				MSG_BOX("Failed Read String Data");
				return E_FAIL;
			}

			// Animation Duration
			ReadFile(hFile, &(pAnimation->Duration), sizeof(_double), &dwByte, nullptr);

			// Animation TickPerSecond
			ReadFile(hFile, &(pAnimation->TickPerSecond), sizeof(_double), &dwByte, nullptr);

			// Animation NumChannels
			ReadFile(hFile, &(pAnimation->NumChannels), sizeof(_uint), &dwByte, nullptr);

			pAnimation->Channels = new CHANNEL[pAnimation->NumChannels];
			ZeroMemory(pAnimation->Channels, sizeof(CHANNEL) * (pAnimation->NumChannels));

			for (_uint j = 0; j < pAnimation->NumChannels; ++j)
			{
				CHANNEL Channel;
				ZEROMEM(&Channel);

				// Animation Name
				ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
				ReadFile(hFile, Channel.Name, dwStrByte, &dwByte, nullptr);
				if (0 == dwByte)
				{
					MSG_BOX("Failed Read String Data");
					return E_FAIL;
				}

				// Channel NumScalingKeys
				ReadFile(hFile, &(Channel.NumScalingKeys), sizeof(_uint), &dwByte, nullptr);

				// Channel ScalingKeys
				Channel.ScalingKeys = new VECTORKEY[Channel.NumScalingKeys];
				ZeroMemory(Channel.ScalingKeys, sizeof(VECTORKEY)* (Channel.NumScalingKeys));
				ReadFile(hFile, Channel.ScalingKeys, sizeof(VECTORKEY) * (Channel.NumScalingKeys), &dwByte, nullptr);

				// Channel NumRotationKeys
				ReadFile(hFile, &(Channel.NumRotationKeys), sizeof(_uint), &dwByte, nullptr);

				// Channel RotationKeys
				Channel.RotationKeys = new QUATERNIONKEY[Channel.NumRotationKeys];
				ZeroMemory(Channel.RotationKeys, sizeof(QUATERNIONKEY)* (Channel.NumRotationKeys));
				ReadFile(hFile, Channel.RotationKeys, sizeof(QUATERNIONKEY) * (Channel.NumRotationKeys), &dwByte, nullptr);

				// Channel NumPositionKeys
				ReadFile(hFile, &(Channel.NumPositionKeys), sizeof(_uint), &dwByte, nullptr);

				// Channel PositionKeys
				Channel.PositionKeys = new VECTORKEY[Channel.NumPositionKeys];
				ZeroMemory(Channel.PositionKeys, sizeof(VECTORKEY)* (Channel.NumPositionKeys));
				ReadFile(hFile, Channel.PositionKeys, sizeof(VECTORKEY) * (Channel.NumPositionKeys), &dwByte, nullptr);

				pAnimation->Channels[j] = Channel;
			}

			m_AnimationDatas.push_back(pAnimation);
		}
	}
	else // NonAnim
	{
		m_Model.NumAnimations = 0;
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CModel::Ready_Bones(Engine::NODE* pNode)
{
	CBone* pBone = CBone::Create(pNode);

	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	for (_uint i = 0; i < pNode->NumChildren; ++i)
	{
		Ready_Bones(m_NodeDatas[pNode->Children[i]]);
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes(TYPE eType, _fmatrix PivotMatrix)
{
	m_iNumMeshes = m_Model.NumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, m_Bones, m_MeshDatas[i], PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}
	return S_OK;
}

HRESULT CModel::Ready_Materials(const _tchar* pModelFilePath)
{
	m_iNumMaterials = m_Model.NumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MESHMATERIAL			MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof MeshMaterial);

		for (_uint j = 0; j < TextureType_MAX; ++j)
		{
			if (!lstrcmp(m_MaterialDatas[i]->MaterialTexture[j].TexPath, TEXT("")))
			{
				if (j == TextureType_DIFFUSE)
				{
					lstrcpy(m_MaterialDatas[i]->MaterialTexture[j].TexPath, TEXT("../../Resources/Terrain/NullDiffuse.png"));
				}
				else
					continue;
			}

			MeshMaterial.pMtrlTexture[j] = CTexture::Create(m_pDevice, m_pContext, 
				m_MaterialDatas[i]->MaterialTexture[j].TexPath, 1);

			if (nullptr == MeshMaterial.pMtrlTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_Model.NumAnimations;

	for (_uint i = 0; i < m_Model.NumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(m_AnimationDatas[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CModel::Release_FileDatas()
{
	Safe_Delete_Array(m_Model.ORMTextures);

	for (auto& pNode : m_NodeDatas)
	{
		Safe_Delete_Array(pNode->Children);

		Safe_Delete(pNode);
	}
	m_NodeDatas.clear();

	for (auto& pMesh : m_MeshDatas)
	{
		for (_uint i = 0; i < pMesh->NumFaces; ++i)
		{
			Safe_Delete_Array(pMesh->Faces[i].Indices);
		}

		Safe_Delete_Array(pMesh->Faces);

		Safe_Delete_Array(pMesh->Positions);
		Safe_Delete_Array(pMesh->Normals);
		Safe_Delete_Array(pMesh->TexCoords);
		Safe_Delete_Array(pMesh->Tangents);

		for (_uint i = 0; i < pMesh->NumBones; ++i)
		{
			Safe_Delete_Array(pMesh->Bones[i].Weights);
		}

		Safe_Delete_Array(pMesh->Bones);

		Safe_Delete(pMesh);
	}
	m_MeshDatas.clear();

	for (auto& pMaterial : m_MaterialDatas)
	{
		Safe_Delete(pMaterial);
	}
	m_MaterialDatas.clear();

	for (auto& pAnimation : m_AnimationDatas)
	{
		for (_uint i = 0; i < pAnimation->NumChannels; ++i)
		{
			Safe_Delete_Array(pAnimation->Channels[i].ScalingKeys);
			Safe_Delete_Array(pAnimation->Channels[i].RotationKeys);
			Safe_Delete_Array(pAnimation->Channels[i].PositionKeys);
		}
		Safe_Delete_Array(pAnimation->Channels);

		Safe_Delete(pAnimation);
	}
	m_AnimationDatas.clear();
}


void CModel::Free()
{
	__super::Free();
	
	if (!m_isCloned)
	{
		Release_FileDatas();
	}

	for (auto& pBone : m_Bones)
	{
		Safe_Release(pBone);
	}
	m_Bones.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTexture)
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pAnimation : m_Animations)
	{
		Safe_Release(pAnimation);
	}
	m_Animations.clear();
}
