// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AAAITypes.h"
#include "AIState.generated.h"

class AEnemyAIControllerBase;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	AS_Passive			UMETA(DisplayName = "Passive"),
	AS_Attacking		UMETA(DisplayName = "Attacking"),
	AS_Frozen			UMETA(DisplayName = "Frozen"), // In a action that is not controlled or executed in the BehaviourTree. Externally activated actions.
	AS_Dead				UMETA(DisplayName = "Dead"),
	AS_MAX				UMETA(DisplayName = "Invalid Enum"),
};

/**
 * 
 */
UCLASS(BlueprintType)
class VERTICALSLICE_API UAIStateLayer : public UObject
{
	GENERATED_BODY()
	
public:
	UAIStateLayer() = default;

	void Init(AEnemyAIControllerBase* InController);

	// Returns false if we can not become this State. 
	virtual bool CanBecomePrimary(AEnemyAIControllerBase* InjectedController) const;

	// Called when the State becomes Primary in the stack.
	virtual void OnBecomePrimary();

	// Called when a new State is assigned as Primary. This can be triggered by another state of the same type!
	virtual void OnRemovedFromPrimary();

	// Called when first added to the stack.
	virtual void OnAdded();
	
	// Called when removed from the stack. This will not be valid after this call!
	virtual void OnRemoved();

	UPROPERTY(BlueprintReadOnly)
	EAIState State = EAIState::AS_MAX;

	TObjectPtr<AEnemyAIControllerBase> Controller = nullptr;
};

UCLASS()
class VERTICALSLICE_API UAIStateLayer_Passive : public UAIStateLayer
{
	GENERATED_BODY()

public:
	UAIStateLayer_Passive();

	bool CanBecomePrimary(AEnemyAIControllerBase* InjectedController) const override;

	void OnBecomePrimary() override;
};

UCLASS()
class VERTICALSLICE_API UAIStateLayer_Attacking : public UAIStateLayer
{
	GENERATED_BODY()

public:
	UAIStateLayer_Attacking();

	bool CanBecomePrimary(AEnemyAIControllerBase* InjectedController) const override;

	void OnBecomePrimary() override;

	void OnAdded() override;

	void OnRemoved() override;

	// The hostile we want to engage in this state. 
	TWeakObjectPtr<AActor> Target;
};

UCLASS()
class VERTICALSLICE_API UAIStateLayer_Frozen : public UAIStateLayer
{
	GENERATED_BODY()

public:
	UAIStateLayer_Frozen();

	bool CanBecomePrimary(AEnemyAIControllerBase* InjectedController) const override;

	void OnBecomePrimary() override;

	void OnRemovedFromPrimary() override;
};

UCLASS()
class VERTICALSLICE_API UAIStateLayer_Dead : public UAIStateLayer
{
	GENERATED_BODY()

public:
	UAIStateLayer_Dead();

	bool CanBecomePrimary(AEnemyAIControllerBase* InjectedController) const override;

	void OnBecomePrimary() override;
};

UCLASS()
class VERTICALSLICE_API UAIStateLayerFactory : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIState")
	static UAIStateLayer* CreateLayer(TSubclassOf<UAIStateLayer> Class, AEnemyAIControllerBase* Controller);

	template<class T>
	static T* CreateLayer(AEnemyAIControllerBase* Controller)
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UAIStateLayer>::Value, "'T' template parameter to CreateLayer must be derived from UAIStateLayer");

		return (T*)CreateLayer(T::StaticClass(), Controller);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIState")
	static UAIStateLayer_Passive*	CreateLayer_Passive(AEnemyAIControllerBase* Controller);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIState")
	static UAIStateLayer_Attacking*	CreateLayer_Attacking(AEnemyAIControllerBase* Controller, AActor* Target = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIState")
	static UAIStateLayer_Frozen*	CreateLayer_Frozen(AEnemyAIControllerBase* Controller);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AIState")
	static UAIStateLayer_Dead*		CreateLayer_Dead(AEnemyAIControllerBase* Controller);

};
