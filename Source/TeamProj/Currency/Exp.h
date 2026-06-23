#pragma once

#include "CoreMinimal.h"
#include "CurrencyBase.h"
#include "Exp.generated.h"
UCLASS()
class TEAMPROJ_API AExp : public ACurrencyBase
{
    GENERATED_BODY()

    protected:
    UPROPERTY(BlueprintReadOnly)
    int32 Exp{ 0 };

    UPROPERTY(EditDefaultsOnly, Category = "Currency|Exp")
    TSoftObjectPtr<UStaticMesh> ExpMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Currency|Exp")
    TSoftObjectPtr<UMaterialInterface> ExpMaterial;

public:
    AExp();

protected:
    virtual void BeginPlay() override;
    virtual void OnGet() override;

public:
    UFUNCTION(BlueprintCallable)
    void GetExp(const int32 InExp);
};