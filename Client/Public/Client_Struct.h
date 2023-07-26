#pragma once
namespace Client
{
	typedef struct tagSkillDesc
	{
		// 스킬 사용을 했는지
		bool		isUsed;
		// 스킬 사용 가능 여부
		bool		isCoolTime;
		// 스킬 유지시간
		double		dDuration;
		// 스킬 사용 후 시간 (Duration을 재기위함.)
		double		dCurTime;

		// 스킬 쿨타임
		double		dOriginCoolTime;
		// 남은 쿨
		double		dCoolTime;
	}SKILLDESC;

	typedef struct tagDashSkill : public tagSkillDesc
	{
		float		fLimitVelocity;
		float		fSpeed;
	}DASHDESC;
}