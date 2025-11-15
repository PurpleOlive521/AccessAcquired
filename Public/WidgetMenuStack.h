// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "WidgetMenuStack.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;

/**
 *  Named WidgetMenuStack to avoid conflicts with Unreals native MenuStack file. 
 */
UCLASS(Blueprintable)
class VERTICALSLICE_API UWidgetMenuStack : public UCommonUserWidget
{
	GENERATED_BODY()

public:

	// Pushes the specified class to the Main Stack. Meant for in-game menus and UI, layered under Menu and Modal.
	// Returns a pointer to the item in the stack.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "Push To Game"), Category = "MenuStack")
	UCommonActivatableWidget* K2_PushToGame(TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass);

	// Pushes the specified class to the Menu Stack. Meant for UI menus. Layered below Modal and above Game.
	// Returns a pointer to the item in the stack.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "Push To Menu"), Category = "MenuStack")
	UCommonActivatableWidget* K2_PushToMenu(TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass);

	// Pushes the specifide class to the Modal Stack. Meant for popups. Layered above ALL other menus.
	// Returns a pointer to the item in the stack.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "Push To Modal"), Category = "MenuStack")
	UCommonActivatableWidget* K2_PushToModal(TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass);

	// Clears the widget in any and all stacks present in this Widget.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MenuStack")
	void ClearAllWidgets();

	// Pushes a Widget of the specified type to the Game Stack. Returns the created widget, or nullptr on failure.
	// @param T : The type of widget to create and push. Must be a subclass of UCommonActivatableWidget.
	template<class T>
	T* PushToGame()
	{
		UClass* ClassToInstantiate = T::StaticClass();
		return static_cast<T*>(K2_PushToGame(ClassToInstantiate));
	}

	// Pushes a Widget of the specified type to the Game Stack. Returns the created widget, or nullptr on failure.
	// @param WidgetClass : The class of the widget to create and push. Must derive from UCommonActivatableWidget!
	// @param T : The type to cast the returned widget object to. 
	template<class TReturnType>
	TReturnType* PushToGame(UClass* WidgetClass)
	{
		return Cast<TReturnType>(K2_PushToGame(WidgetClass));
	}

	// Pushes a Widget of the specified type to the Menu Stack. Returns the created widget, or nullptr on failure.
	// @param T : The type of widget to create and push. Must be a subclass of UCommonActivatableWidget.
	template<class T>
	T* PushToMenu()
	{
		UClass* ClassToInstantiate = T::StaticClass();
		return static_cast<T*>(K2_PushToMenu(ClassToInstantiate));
	}

	// Pushes a Widget of the specified type to the Modal Stack. Returns the created widget, or nullptr on failure.
	// @param T : The type of widget to create and push. Must be a subclass of UCommonActivatableWidget.
	template<class T>
	T* PushToModal()
	{
		UClass* ClassToInstantiate = T::StaticClass();
		return static_cast<T*>(K2_PushToModal(ClassToInstantiate));
	}
};
