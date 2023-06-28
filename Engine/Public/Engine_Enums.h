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
		CELL_MOVE		= 0x0001,
		// 특정 Y값보다 내려가면 사망처리되는 떨어지는셀.
		CELL_FALL		= 0x0002,
		// 벽을 타고 달릴수있게 처리한 셀
		CELL_WALL		= 0x0004,
		// Y값이 아래에서 해당셀로 넘어가려할때 일정Y값이면 오르는 모션과함께
		// 셀위로 올라가지는 셀
		CELL_CLIMB		= 0x0008,
		
	};
}
