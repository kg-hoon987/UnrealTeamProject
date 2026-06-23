// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SkillEvent.h"
#include "GameFramework/Actor.h"
#include "../Character/GameCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Skill/SkillComponent.h"

bool UAnimNotify_SkillEvent::Received_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const
{
    UE_LOG(LogTemp, Warning, TEXT("Notify Start"));
    if (!MeshComp) return false;

    AActor* Root = MeshComp->GetOwner();
    while (Root && Root->GetAttachParentActor())
        Root = Root->GetAttachParentActor();

    if (USkillComponent* Comp = (Root ? Root->FindComponentByClass<USkillComponent>() : nullptr))
    {
        // EventName이 "Fire"일 때만 발사
        if (EventName.IsNone() || EventName == TEXT("Fire"))
            Comp->FirePendingSkillNow();
        return true;
    }
    return false;
}
