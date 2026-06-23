// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "GenericTeamAgentInterface.h"
#include "TeamFilterUtil.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ETargetAffiliation : uint8
{
	Any,			//대상 제한없음 타겟지정, 장판, 등
	HostileOnly,	//적만 타겟 (공격, 디버프용)
	FriendlyOnly,	//우호대상만 ㅇㅇ
	SameTeamOnly,	//같은 진영만 아군이면 아군끼리만
	NotSelf,		//자기자신제외하고 나머지 (파티버프에서 시전자만 제외정도)
	SelfOnly		//자기자신만 (힐 ,버프?)
};


namespace TeamRules
{
	inline bool InPAB(uint8 Id) // Player/Ally/Building 묶음
	{
		return Id == 1 || Id == 2 || Id == 3; // Player=1, Ally=2, Building=3, Enemy=4
	}

	inline ETeamAttitude::Type SolveAttitude(FGenericTeamId A, FGenericTeamId B)
	{
		if (A == B) return ETeamAttitude::Friendly;
		if (InPAB(A.GetId()) && InPAB(B.GetId())) return ETeamAttitude::Friendly;
		return ETeamAttitude::Hostile;
	}
}


namespace TeamFilterUtil
{

	inline FGenericTeamId GetTeamIdOf(const AActor* Actor)
	{
		if (!Actor) return FGenericTeamId::NoTeam;
		if (const APawn* Pawn = Cast<APawn>(Actor))
			if (const AController* C = Pawn->GetController())
				if (const IGenericTeamAgentInterface* TI = Cast<IGenericTeamAgentInterface>(C))
					return TI->GetGenericTeamId();

		if (const IGenericTeamAgentInterface* TI = Cast<IGenericTeamAgentInterface>(Actor))
			return TI->GetGenericTeamId();

		return FGenericTeamId::NoTeam;
	}


	//이건 캐스터의 컨트롤러가 오버라이드한 GetTeamAttitudeTowards를 먼저 사ㅇ용
	inline ETeamAttitude::Type QueryAttitudeController(AActor* Caster, const AActor* Other)
	{
		if (!Caster || !Other) return ETeamAttitude::Neutral;

		//1) 컨트롤러 우선
		if (const APawn* Pawn = Cast<APawn>(Caster))
		{
			if (AController* Ctrl = Pawn->GetController())
				if (const IGenericTeamAgentInterface* TeamCtrl = Cast<IGenericTeamAgentInterface>(Ctrl))
				{
					const ETeamAttitude::Type Att = TeamCtrl->GetTeamAttitudeTowards(*Other);
					if (Att != ETeamAttitude::Neutral) return Att;
				}
		}
		else if (const AController* Ctrl = Cast<AController>(Caster))
		{
			if (const IGenericTeamAgentInterface* TeamCtrl = Cast<IGenericTeamAgentInterface>(Ctrl))
			{
				const ETeamAttitude::Type Att = TeamCtrl->GetTeamAttitudeTowards(*Other);
				if (Att != ETeamAttitude::Neutral) return Att;
			}
		}

		//2) 풀백 : 액터의 IGeneicTeamd~~~~~ 
		return TeamRules::SolveAttitude(GetTeamIdOf(Caster), GetTeamIdOf(Other));
	}


	inline bool PassAffiliation(AActor* Caster, AActor* Target, ETargetAffiliation Mode) //같은 팀만 힐, 적만 공격
	{
		if (!Caster || !Target) return false;

		if (Mode == ETargetAffiliation::SelfOnly) return Target == Caster;
		if (Mode == ETargetAffiliation::NotSelf) return Target != Caster;
		if (Mode == ETargetAffiliation::Any) return true;

		//SameTeamOnly -> 팀 ID 동일 여부만 볼떄
		if (Mode == ETargetAffiliation::SameTeamOnly)
			return TeamFilterUtil::GetTeamIdOf(Caster) == TeamFilterUtil::GetTeamIdOf(Target);

		const ETeamAttitude::Type Att = QueryAttitudeController(Caster, Target);
		if (Mode == ETargetAffiliation::HostileOnly)  return Att == ETeamAttitude::Hostile;
		if (Mode == ETargetAffiliation::FriendlyOnly) return Att == ETeamAttitude::Friendly;

		return true;

	}
}
