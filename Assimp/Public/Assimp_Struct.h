#pragma once
namespace Converter
{
	typedef struct tagFloat2
	{
		float x, y;
	}FLOAT2;

	typedef struct tagFloat3
	{
		float x, y, z;
	}FLOAT3;
	
	typedef struct tagFloat4
	{
		float x, y, z, w;
	}FLOAT4;

	typedef struct tagMatrix4x4
	{
		float _11, _12, _13, _14;
		float _21, _22, _23, _24;
		float _31, _32, _33, _34;
		float _41, _42, _43, _44;
	}MATRIX4X4;

	typedef struct tagNode
	{
		wchar_t				Name[256];
		MATRIX4X4			Transformation; // row major

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
		MATRIX4X4			OffsetMatrix; // row major
		unsigned int		NumWeights;
		WEIGHT*				Weights; // array
	}BONE;

	typedef struct tagMesh
	{
		wchar_t				Name[256];
		unsigned int		MaterialIndex;
		unsigned int		NumVertices;
		unsigned int		NumIndices;
		FLOAT3*				Positions; // array
		FLOAT3*				Normals; // array
		FLOAT2*				TexCoords; // array
		FLOAT3*				Tangents; // array

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
		unsigned int		NumMeshes;
		MESH*				Meshes; // array
		unsigned int		NumMaterials;
		MATERIAL*			Materials; // array
	}MODEL;
}