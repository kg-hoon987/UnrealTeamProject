// Fill out your copyright notice in the Description page of Project Settings.


#include "BPDComponent_Barrack.h"
#include "../Barrack.h"
#include "../../Character/GameCharacterBase.h"
#include "../../Status/StatusComponent.h"
#include "../../Stage/RoundComponent.h"
#include "../../MyGamePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "../../AICon/AllyAIController.h"

UBPDComponent_Barrack::UBPDComponent_Barrack()
{
    PrimaryComponentTick.bCanEverTick = true;
	OwnEffect = { EBuildingEffectType::Barrack_SpawnAlly };
    AllyCount = 0;
    Regentime = 0.f;
    ClassId = 0;

    SpawnCount = 0;
    SpawnedList.Empty();

    AllyBP = StaticLoadClass(AAllyCharacter::StaticClass(), nullptr, TEXT("/Game/ThirdPerson/Blueprints/BP_Ally.BP_Ally_C"));

}

void UBPDComponent_Barrack::InitChild()
{
    for (FBuildingEffect ef : Effects)
    {
        if (CheckEffect(ef))
        {

            OnActivate(ef); 
        }
    }

    if (ABarrack* owner = Cast<ABarrack>(GetOwner()))
    {
        SpawnPos.Empty();
        SpawnPos = owner->CurSpawnPos;
    }
}

void UBPDComponent_Barrack::BeginPlay()
{
    Super::BeginPlay();

    APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Pawn)
    {
        if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
        {
            Roundcom = RoundComp;
        }
    }
}

void UBPDComponent_Barrack::ActivateEffect(const FBuildingEffect& Effect)
{
    AllyCount = Effect.intValue;
    Regentime = Effect.floatValue_1;
}

void UBPDComponent_Barrack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


    if (Roundcom->RoundState == ERoundState::WarStart)
    {
        for (int32 i = 0; i < AllyCount; i++)
        {
            FUnitSlot& Slot = SpawnedList[i];

            if (Slot.isDead)
            {
                if (!Slot.Unit && Slot.RespawnTime > 0.f)
                {
                    Slot.RespawnTime -= DeltaTime;

                    if (Slot.RespawnTime <= 0.f)
                    {
                        SpawnAlly(i);
                    }
                }
            }
        }
    }
}

void UBPDComponent_Barrack::End()
{
    if (SpawnedList.Num() > 0)
    {
        for (FUnitSlot slot : SpawnedList)
        {
            if(slot.Unit!=nullptr)
                slot.Unit->Destroy();
        }
        SpawnedList.Empty();
    }
}

void UBPDComponent_Barrack::SetClassId(int32 id)
{
    ClassId = id;
}

void UBPDComponent_Barrack::InitLRCharacter(EIdentity EIden)
{
    if (!CharInfoTable) 
    {
       UE_LOG(LogTemp, Warning, TEXT("[BPDC_Barrack] CharInfoTable is null."));
       return;
    }

    if (CurrentLevel>0) 
    {
        IdenList.Empty();
        // 데이터 테이블 짝수 = L / 홀수 = R 정보
        for (int i = 0; i < 2; i++)
        {
            int id = ClassId + (CurrentLevel-1) * 2 + i;        // 나중에 데이터테이블 레벨별로 아군 늘릴 때 쓰기.
            FCharInfo* Identity = CharInfoTable->FindRow<FCharInfo>(*FString::FromInt(id), TEXT("GetRow"));
            IdenList.Add(Identity);
            if (!IdenList[i])
            {
                UE_LOG(LogTemp, Warning, TEXT("[BPDC_Barrack] TableRow is Wrong."));
                return;
            }
        }
        SetAllyIdentity(EIden);
    }
}

void UBPDComponent_Barrack::SpawnAlly(int index)
{
    FActorSpawnParameters SpawnPara;
    SpawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    AAllyCharacter* AC;
    FRotator rot = GetOwner()->GetActorRotation() + FRotator(0.f, 180.f, 0.f);
    if (SpawnCount < AllyCount)
    {
        AC = GetWorld()->SpawnActor<AAllyCharacter>(AllyBP, SpawnPos[SpawnCount], rot, SpawnPara);
        }
    else
    {
        AC = GetWorld()->SpawnActor<AAllyCharacter>(AllyBP, SpawnPos[index], rot, SpawnPara);
    }
    if (!AC)
    {
        UE_LOG(LogTemp, Error, TEXT("Spawn failed!"));
        return;
    }
    AC->InitCharInfo(CurSpawnData.ClassType, CurSpawnData.StatID, CurSpawnData.SkillID
        , CurSpawnData.SkillLevel, CurSpawnData.Chest, CurSpawnData.Parts, CurSpawnData.WeaponLeft, CurSpawnData.WeaponRight
        , CurSpawnData.AnimClass, CurSpawnData.CharScale);
    
    // 태그 뭐줘야하는거즤...
    AC->Tags.AddUnique(CurSpawnData.StatID.WeaponType);

    

    Cast<UStatusComponent>(AC->StatusComp)->OnDied.AddDynamic(this, &UBPDComponent_Barrack::OnAllyDied);

    if (SpawnedList.Num() < AllyCount) 
    {
        FUnitSlot slot;
        slot.Unit = AC;
        slot.RespawnTime = Regentime;
        SpawnedList.Add(slot);
        AC->BarrackUnitID = SpawnedList.Num()-1;
        SpawnCount++;
    }
    else
    {
        AC->BarrackUnitID = index;
        SpawnedList[index].Unit = AC;
        SpawnedList[index].RespawnTime = Regentime;
        SpawnedList[index].isDead = false;
    }
}

void UBPDComponent_Barrack::OnAllyDied(AActor* Ally)
{
    AAllyCharacter* ally = Cast<AAllyCharacter>(Ally);
    int32 Index = ally->BarrackUnitID;
    FUnitSlot& Slot = SpawnedList[Index];

    Slot.Unit = nullptr;
    Slot.isDead = true; 
}

void UBPDComponent_Barrack::SetAllyIdentity(EIdentity EIden)
{
    //스폰된 자식새끼들 디스트로이
    if (SpawnedList.Num() > 0)
    {
        for (FUnitSlot slot : SpawnedList)
        {
            slot.Unit->Destroy();
        }
        SpawnedList.Empty();
        SpawnCount = 0;
    }
    //골라진 아덴으로 스폰할 액터 데이터 설정
    switch (EIden)
    {
    case EIdentity::L:
        CurSpawnData = *IdenList[0];
        break;
    case EIdentity::R:
        CurSpawnData = *IdenList[1];
        break;
    default:
        break;
    }

  
    //스폰 수만큼 바로 스폰

    for (int i = 0; i < AllyCount; i++)
    {
        SpawnAlly();
    }
}


