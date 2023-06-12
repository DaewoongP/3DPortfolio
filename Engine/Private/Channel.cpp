#include "..\Public\Channel.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel)
{
	strcpy_s(m_szName, MAX_PATH, pAIChannel->mNodeName.data);

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3 vScale;
	ZEROMEM(&vScale);
	_float4 vRotation;
	ZEROMEM(&vRotation);
	_float3 vPosition;
	ZEROMEM(&vPosition);

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME	KeyFrame;

		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof _float3);

			KeyFrame.dTime = pAIChannel->mScalingKeys[i].mTime;
		}

		if (pAIChannel->mNumRotationKeys > i)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

			KeyFrame.dTime = pAIChannel->mRotationKeys[i].mTime;
		}

		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof _float3);

			KeyFrame.dTime = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(_double dTimeAcc)
{
	KEYFRAME LastKeyFrame = m_KeyFrames.back();

	_vector			vScale;
	_vector			vRotation;
	_vector			vTranslation;

	if (dTimeAcc >= LastKeyFrame.dTime)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMLoadFloat3(&LastKeyFrame.vTranslation);
	}
	else
	{
		if (dTimeAcc >= m_KeyFrames[m_iCurrentKeyFrame + 1].dTime)
			++m_iCurrentKeyFrame;

		_double dRatio = (dTimeAcc - m_KeyFrames[m_iCurrentKeyFrame].dTime)
			/ (m_KeyFrames[m_iCurrentKeyFrame + 1].dTime - m_KeyFrames[m_iCurrentKeyFrame].dTime);

		_vector vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame].vScale);
		_vector vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vRotation);
		_vector vSourTraslation = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame].vTranslation);
		
		_vector vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1].vScale);
		_vector vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1].vRotation);
		_vector vDestTraslation = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1].vTranslation);

		vScale = XMVectorLerp(vSourScale, vDestScale, dRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, dRatio);
		vTranslation = XMVectorLerp(vSourTraslation, vDestTraslation, dRatio);
	}

	_matrix TransformationMatrix;
	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
	// 반환해야함.
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel)
{
	CChannel* pInstance = new CChannel();
	if (FAILED(pInstance->Initialize(pAIChannel)))
	{
		MSG_BOX("Failed to Created CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
}
