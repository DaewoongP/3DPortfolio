#pragma once
namespace Client
{
	typedef struct tagSkillDesc
	{
		// ��ų ��뿩��
		bool		isUsed;
		// ��ų �����ð�
		double		dDuration;
		// ��ų ��� �� �ð� (Duration�� �������.)
		double		dCurTime;

		// ��ų ��Ÿ��
		double		dOriginCoolTime;
		// ���� ��
		double		dCoolTime;
	}SKILLDESC;

	typedef struct tagDashSkill : public tagSkillDesc
	{
		float		fLimitVelocity;
		float		fSpeed;
	}DASHDESC;
}