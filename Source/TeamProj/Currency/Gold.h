#pragma once

#include "CoreMinimal.h"
#include "CurrencyBase.h"
#include "Gold.generated.h"

UCLASS()
class TEAMPROJ_API AGold : public ACurrencyBase
{
    GENERATED_BODY()

    protected:
    UPROPERTY(BlueprintReadOnly)
    int32 Money{ 0 };

    UPROPERTY(EditDefaultsOnly, Category = "Currency|Gold")
    TSoftObjectPtr<UStaticMesh> MeshSmall;
    UPROPERTY(EditDefaultsOnly, Category = "Currency|Gold")
    TSoftObjectPtr<UStaticMesh> MeshMid;
    UPROPERTY(EditDefaultsOnly, Category = "Currency|Gold")
    TSoftObjectPtr<UStaticMesh> MeshBig;
    UPROPERTY(EditDefaultsOnly, Category = "Currency|Gold")
    TSoftObjectPtr<UStaticMesh> MeshBag;

public:
    AGold();

protected:
    virtual void BeginPlay() override;
    virtual void OnGet() override;

public:
    UFUNCTION(BlueprintCallable)
    void GetMoney(int32 InMoney);
};