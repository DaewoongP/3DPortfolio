#pragma once
namespace Tool
{
	static bool IsMouseInMainWindow(HWND hWnd, int iWindowX, int iWindowY)
	{
		// ����Ʈ ��ǥ�� �޾ƿ;���.
		POINT	pt{};
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);

		if (pt.x < 0 ||
			pt.y < 0 ||
			pt.x > iWindowX ||
			pt.y > iWindowY)
			return false;
		else
			return true;
	}







}