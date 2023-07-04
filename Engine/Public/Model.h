#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_float Get_CurrentFramePercent();
	_float4x4 Get_PivotFloat4x4() const { return m_PivotMatrix; }
	_matrix Get_PivotMatrix() { return XMLoadFloat4x4(&m_PivotMatrix); }
	_uint Get_CurrentAnimIndex() const { return m_iCurrentAnimIndex; }
	ANIMATIONFLAG Get_AnimationState() const { return m_eAnimationFlag; }
	// 모델의 메쉬 개수 반환
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	// 모델의 애니메이션 개수 반환
	_uint Get_NumAnimations() const { return m_iNumAnimations; }
	const class CBone* Get_Bone(const _tchar* pBoneName);
	// 현재 실행중인 애니메이션
	class CAnimation* Get_Animation() const { return m_Animations[m_iCurrentAnimIndex]; }
	class CAnimation* Get_Animation(_uint iAnimationIndex) const { return m_Animations[iAnimationIndex]; }
	_float4x4 Get_BoneCombinedTransformationMatrix(_uint iIndex);
	// 애니메이션 인덱스 설정하면서 루프도 같이 설정
	void Set_AnimIndex(_uint iAnimIndex, _bool isLoop = true);
	// 현재 키프레임을 변경
	void Set_CurrentKeyFrameIndex(_uint iKeyFrameIndex);

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void	Play_Animation(_double dTimeDelta, _bool isPlaying = true, _double dLerpDuration = 0.2);
	void	Invalidate_AnimationCamera(class CCamera* pCamera, class CTransform* pPlayerTransform, _double dTimeDelta);
	HRESULT Find_BoneIndex(const _tchar* pBoneName, _uint* iIndex);
	HRESULT SetUp_AnimationNotifies(_uint iAnimationIndex, vector<NOTIFY> Notifies);

public:
	HRESULT Bind_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, Engine::TextureType MaterialType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);

private: // Files
	Engine::MODEL				m_Model;
	vector<Engine::NODE*>		m_NodeDatas;
	vector<Engine::MESH*>		m_MeshDatas;
	vector<Engine::MATERIAL*>	m_MaterialDatas;
	vector<Engine::ANIMATION*>	m_AnimationDatas;

private: /* For.Bones */
	vector<class CBone*>		m_Bones;
public:
	typedef vector<class CBone*>	BONES;

private: /* For.Meshes */
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

private: /* For.Materials */
	_uint						m_iNumMaterials = { 0 };
	vector<MESHMATERIAL>		m_Materials;

private: /* For.Animations */
	_uint							m_iPreviousAnimIndex = { 0 };
	_uint							m_iCurrentAnimIndex = { 0 };
	ANIMATIONFLAG					m_eAnimationFlag = { ANIM_END };
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;

private:
	_float4x4						m_PivotMatrix;

private:
	HRESULT Ready_File(TYPE eType, const _tchar* pModelFilePath);
	HRESULT Ready_Bones(Engine::NODE* pNode);
	HRESULT Ready_Meshes(TYPE eType, _fmatrix PivotMatrix);
	HRESULT Ready_Materials(const _tchar* pModelFilePath);
	HRESULT Ready_Animations();

private:
	void Release_FileDatas();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END