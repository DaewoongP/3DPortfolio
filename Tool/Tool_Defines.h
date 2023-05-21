#pragma once
namespace Tool
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum class LEVELID { LEVEL_STATIC, LEVEL_END };
}

extern HWND g_hViewWnd;

using namespace Tool;