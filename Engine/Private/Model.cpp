#include "..\Public\Model.h"
#include "Bone.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
	ZEROMEM(&m_Model);
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_Model(rhs.m_Model)
	, m_Nodes(rhs.m_Nodes)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
{
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix)
{
	if (FAILED(Ready_File(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Bones(m_Nodes.front(), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
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

	DWORD	dwByte = 0;
	DWORD	dwStrByte = 0;

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

		for (_uint j = 0; j < pNode->NumChildren; ++j)
		{
			ReadFile(hFile, &pNode->Children[j], sizeof(_uint), &dwByte, nullptr);
		}

		m_Nodes.push_back(pNode);
	}

	// Write Meshes

	// Meshes NumMeshes
	ReadFile(hFile, &(m_Model.NumMeshes), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.NumMeshes; i++)
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

		// Mesh NumIndices
		ReadFile(hFile, &(pMesh->NumIndices), sizeof(_uint), &dwByte, nullptr);

		// Mesh Positions
		pMesh->Positions = new _float3[pMesh->NumVertices];
		pMesh->Normals = new _float3[pMesh->NumVertices];
		pMesh->TexCoords = new _float2[pMesh->NumVertices];
		pMesh->Tangents = new _float3[pMesh->NumVertices];
		for (_uint j = 0; j < pMesh->NumVertices; ++j)
		{
			ReadFile(hFile, &pMesh->Positions[j], sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->Normals[j], sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->TexCoords[j], sizeof(_float2), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->Tangents[j], sizeof(_float3), &dwByte, nullptr);
		}

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

		m_Meshes.push_back(pMesh);
	}

	// Write Materials

	// Material NumMaterials
	ReadFile(hFile, &(m_Model.NumMaterials), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_Model.NumMaterials; i++)
	{
		MATERIAL* pMaterial = new MATERIAL;

		// MaterialTex
		ReadFile(hFile, pMaterial->MaterialTexture, sizeof(MATERIALTEX) * TextureType_MAX, &dwByte, nullptr);

		m_Materials.push_back(pMaterial);
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
		Ready_Bones(m_Nodes[pNode->Children[i]], pBone);
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

	for (auto& pNode : m_Nodes)
	{
		Safe_Delete_Array(pNode->Children);
		
		Safe_Delete(pNode);
	}
	m_Nodes.clear();

	for (auto& pMesh : m_Meshes)
	{
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
	m_Meshes.clear();

	for (auto& pMaterial : m_Materials)
	{
		Safe_Delete(pMaterial);
	}
	m_Materials.clear();
}
