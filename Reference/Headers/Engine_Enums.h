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
		// �Ϲ� �̵������� ��
		CELL_MOVE,
		// Ư�� Y������ �������� ���ó���Ǵ� �������¼�.
		CELL_FALL,
		// �������� ���ϴ� �� (����������)
		CELL_STATIC,
		// �����̵� ��
		CELL_SLIDE
	};

	// Late Tick�� ����Ǹ� ���� ���¸� ��ȯ�Ͽ� ó��
	enum GAMEEVENT
	{
		GAME_NOEVENT,
		GAME_OBJECT_DEAD,
		GAME_STAGE_RESET,
		GAME_STAGE_END,
		GAME_END
	};

}
