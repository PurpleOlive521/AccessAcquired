// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "LevelTypes.h"

#include "LevelStreamingTrigger.generated.h"


UCLASS()
class VERTICALSLICE_API ALevelStreamingTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelStreamingTrigger();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ELevelName> LevelsToLoad;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ELevelName> LevelsToUnload;

};
