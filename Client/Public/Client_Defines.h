#pragma once
#include "Process.h"
namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_STAGE1, LEVEL_STAGE2, LEVEL_BOSS, LEVEL_LOADING, LEVEL_END };
}

#include "Client_Struct.h"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern double g_TimeDelta;

using namespace Client;