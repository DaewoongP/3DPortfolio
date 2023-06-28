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
		CELL_MOVE		= 0x0001,
		// Ư�� Y������ �������� ���ó���Ǵ� �������¼�.
		CELL_FALL		= 0x0002,
		// ���� Ÿ�� �޸����ְ� ó���� ��
		CELL_WALL		= 0x0004,
		// Y���� �Ʒ����� �ش缿�� �Ѿ���Ҷ� ����Y���̸� ������ ��ǰ��Բ�
		// ������ �ö����� ��
		CELL_CLIMB		= 0x0008,
		
	};
}
