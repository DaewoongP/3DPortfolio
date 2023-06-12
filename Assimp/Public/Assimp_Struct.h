#pragma once
namespace Converter
{
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
		MATRIX4X4			Transformation;

		unsigned int		NodeIndex;
		unsigned int		Parent;
		unsigned int		NumChildren;
		unsigned int*		Children;
	}NODE;
}