#include "..\Public\Model.h"
#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

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
	, m_Bones(rhs.m_Bones)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
{
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix)
{
	if (FAILED(Ready_File(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Bones(m_NodeDatas.front(), nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(eType, PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
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
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix();
	}
}

HRESULT CModel::Bind_Material(CShader* pShader, const char* pConstantName, _uint iMeshIndex, Engine::TextureType MaterialType)
{
	if (iMeshIndex >= m_iNumMeshes ||
		MaterialType >= TextureType_MAX)
		return E_FAIL;

	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMtrlTexture[MaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const char* pConstantName, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[256];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 256);

	m_Meshes[iMeshIndex]->Get_Matrices(m_Bones, BoneMatrices);

	pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);

	return S_OK;
}

HRESULT CModel::Ready_File(const _tchar* pModelFilePath)
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
		ReadFile(hFile, &(pNode->Parent), sizeof(_uint), &dwByte, nullptr);

		// Node NumChildren
		ReadFile(hFile, &(pNode->NumChildren), sizeof(_uint), &dwByte, nullptr);

		// Node Children (array)
		pNode->Children = new _uint[pNode->NumChildren];
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

		for (_uint j = 0; j < pMesh->NumFaces; ++j)
		{
			FACE Face;
			ZEROMEM(&Face);

			// Face NumIndices
			ReadFile(hFile, &(Face.NumIndices), sizeof(_uint), &dwByte, nullptr);

			// Face Indices
			Face.Indices = new _uint[Face.NumIndices];
			ReadFile(hFile, Face.Indices, sizeof(_uint) * (Face.NumIndices), &dwByte, nullptr);
			
			pMesh->Faces[j] = Face;
		}

		// Mesh Positions
		pMesh->Positions = new _float3[pMesh->NumVertices];
		ReadFile(hFile, pMesh->Positions, sizeof(_float3) * (pMesh->NumVertices), &dwByte, nullptr);

		// Mesh Normals
		pMesh->Normals = new _float3[pMesh->NumVertices];
		ReadFile(hFile, pMesh->Normals, sizeof(_float3) * (pMesh->NumVertices), &dwByte, nullptr);

		// Mesh TexCoords
		pMesh->TexCoords = new _float2[pMesh->NumVertices];
		ReadFile(hFile, pMesh->TexCoords, sizeof(_float2) * (pMesh->NumVertices), &dwByte, nullptr);

		// Mesh Tangents
		pMesh->Tangents = new _float3[pMesh->NumVertices];
		ReadFile(hFile, pMesh->Tangents, sizeof(_float3) * (pMesh->NumVertices), &dwByte, nullptr);

		// Mesh NumBones
		ReadFile(hFile, &(pMesh->NumBones), sizeof(_uint), &dwByte, nullptr);

		pMesh->Bones = new BONE[pMesh->NumBones];

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

	// Write Materials

	// Material NumMaterials
	ReadFile(hFile, &(m_Model.NumMaterials), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.NumMaterials; i++)
	{
		MATERIAL* pMaterial = new MATERIAL;

		// MaterialTex
		ReadFile(hFile, pMaterial->MaterialTexture, sizeof(MATERIALTEX) * TextureType_MAX, &dwByte, nullptr);

		m_MaterialDatas.push_back(pMaterial);
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CModel::Ready_Bones(Engine::NODE* pNode, CBone* pParent)
{
	CBone* pBone = CBone::Create(pNode, pParent);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	for (size_t i = 0; i < pNode->NumChildren; ++i)
	{
		Ready_Bones(m_NodeDatas[pNode->Children[i]], pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes(TYPE eType, _fmatrix PivotMatrix)
{
	m_iNumMeshes = m_Model.NumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; ++i)
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

	for (size_t i = 0; i < m_iNumMaterials; ++i)
	{
		MESHMATERIAL			MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof MeshMaterial);

		for (size_t j = 0; j < TextureType_MAX; ++j)
		{
			if (!lstrcmp(m_MaterialDatas[i]->MaterialTexture[j].TexPath, TEXT("")))
				continue;

			MeshMaterial.pMtrlTexture[j] = CTexture::Create(m_pDevice, m_pContext, 
				m_MaterialDatas[i]->MaterialTexture[j].TexPath, 1);

			if (nullptr == MeshMaterial.pMtrlTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(MeshMaterial);
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

void CModel::Free()
{
	__super::Free();
	// delete
	if (m_bIsClone)
		return;

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
}
