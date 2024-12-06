// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitlePC.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API ATitlePC : public APlayerController
{
	GENERATED_BODY()
public:
	ATitlePC();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "UI")
	TSubclassOf<class UTitleWidgetBase> TitleWidgetClass;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	class UTitleWidgetBase* TitleWidgetObject;

	UFUNCTION(BlueprintCallable)
	void ShowInputRoomNumberPopup(bool Show);

	void ShowFriendPopUp(bool Show);

	void SetClickedButton();

	UFUNCTION(BlueprintCallable)
	void ShowRoomErrorText(int32 RoomNum);

	UFUNCTION(BlueprintCallable)
	void ShowAddFriend(TArray<int32> SuccessArr, TArray<FString> NicknameArr);

	UFUNCTION(BlueprintCallable)
	void ShowInvitationList(int32 RoomNumber, FString Nickname);

	UFUNCTION(BlueprintCallable)
	void ShowRoomList(int32 RoomNumber, FString Nickname);

	UFUNCTION(BlueprintCallable)
	void ShowRoomNotExist(bool Show);

#pragma region Option

private:
	void OnBackButton();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> SystemMenuIMC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> BackAction;

#pragma endregion Option

};
