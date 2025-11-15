// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DynamicCameraPerspective.generated.h"

UCLASS()
class VERTICALSLICE_API ADynamicCameraPerspective : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADynamicCameraPerspective();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "DynamicCamera")
	TSet<TObjectPtr<USceneComponent>> AvailableBlendPoints;

	UPROPERTY(BlueprintReadWrite, Category = "DynamicCamera")
	USceneComponent* ClosestPoint;

	UFUNCTION(BlueprintCallable, Category = "DynamicCamera")
	void GetClosestBlendPoint(AActor* TriggerActor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
