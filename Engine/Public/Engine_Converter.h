#pragma once
#include <DirectXMath.h>
using namespace DirectX;

namespace Engine
{
	typedef struct tagNode
	{
		wchar_t				Name[256];
		XMFLOAT4X4			Transformation; // row major

		unsigned int		NodeIndex;
		unsigned int		Parent;
		unsigned int		NumChildren;
		unsigned int*		Children; // array
	}NODE;

	typedef struct tagWeight
	{
		unsigned int		VertexId;
		float				Weight;
	}WEIGHT;

	typedef struct tagBone
	{
		wchar_t				Name[256];
		XMFLOAT4X4			OffsetMatrix; // row major
		unsigned int		NumWeights;
		WEIGHT*				Weights; // array
	}BONE;

	typedef struct tagFace
	{
		unsigned int		NumIndices;
		unsigned int*		Indices; // array
	}FACE;

	typedef struct tagMesh
	{
		wchar_t				Name[256];
		unsigned int		MaterialIndex;
		unsigned int		NumVertices;
		unsigned int		NumFaces;
		FACE*				Faces; // array
		XMFLOAT3*			Positions; // array
		XMFLOAT3*			Normals; // array
		XMFLOAT2*			TexCoords; // array
		XMFLOAT3*			Tangents; // array

		unsigned int		NumBones;
		BONE*				Bones; // array
	}MESH;

	typedef enum TextureType
	{
		TextureType_NONE,
		TextureType_DIFFUSE,
		TextureType_SPECULAR,
		TextureType_AMBIENT,
		TextureType_EMISSIVE,
		TextureType_HEIGHT,
		TextureType_NORMALS,
		TextureType_SHININESS,
		TextureType_OPACITY,
		TextureType_DISPLACEMENT,
		TextureType_LIGHTMAP,
		TextureType_REFLECTION,
		TextureType_BASE_COLOR,
		TextureType_NORMAL_CAMERA,
		TextureType_EMISSION_COLOR,
		TextureType_METALNESS,
		TextureType_DIFFUSE_ROUGHNESS,
		TextureType_AMBIENT_OCCLUSION,
		TextureType_SHEEN,
		TextureType_CLEARCOAT,
		TextureType_TRANSMISSION,
		TextureType_UNKNOWN,
	}TEXTYPE;
#define TextureType_MAX TextureType_UNKNOWN

	typedef struct tagTexture
	{
		TEXTYPE				TexType;
		wchar_t				TexPath[256];
	}MATERIALTEX;

	typedef struct tagMaterial
	{
		MATERIALTEX			MaterialTexture[TextureType_MAX];
	}MATERIAL;

	typedef struct tagModel
	{
		unsigned int		NumNodes;

		unsigned int		NumMeshes;
		MESH*				Meshes; // array
		unsigned int		NumMaterials;
		MATERIAL*			Materials; // array
	}MODEL;
}