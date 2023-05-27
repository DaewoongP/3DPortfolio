#include "..\Public\Level_GamePlay.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
}

HRESULT CLevel_GamePlay::Render()
{
	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

void CLevel_GamePlay::Free()
{
}
