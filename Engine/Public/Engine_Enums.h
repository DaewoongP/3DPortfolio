#pragma once

namespace Engine
{
	enum ANIMATIONFLAG
	{
		ANIM_PLAYING,
		ANIM_FINISHED,
		ANIM_LERP,
		ANIM_LERP_FINISHED,
		ANIM_PAUSED,
		ANIM_END
	};

	enum CELLFLAG
	{
		// 일반 이동가능한 셀
		CELL_MOVE,
		// 특정 Y값보다 내려가면 사망처리되는 떨어지는셀.
		CELL_FALL,
		// 움직이지 못하는 셀 (지형같은곳)
		CELL_STATIC,
		// 슬라이딩 셀
		CELL_SLIDE
	};

	// Late Tick이 종료되면 현재 상태를 반환하여 처리
	enum GAMEEVENT
	{
		GAME_NOEVENT,
		GAME_OBJECT_DEAD,
		GAME_STAGE_RESET,
		GAME_STAGE_END,
		GAME_END
	};

}
