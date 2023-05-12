#pragma once
namespace Tool
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_LOADING, LEVEL_END };
}

extern HWND g_hWnd;

using namespace Tool;