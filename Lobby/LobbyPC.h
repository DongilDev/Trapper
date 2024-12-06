// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPC.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ALobbyPC : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI")
	class ULobbyWidgetBase* LobbyWidgetObject;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "UI")
	TSubclassOf<class ULobbyWidgetBase> LobbyWidgetClass;

	// 로비 캐릭터 1p
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMesh1;

	// 로비 캐릭터 2p
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMesh2;

	UFUNCTION(Server, Reliable, WithValidation)
	void C2S_SendMessage(const FText& Message);
	bool C2S_SendMessage_Validate(const FText& Message);
	void C2S_SendMessage_Implementation(const FText& Message);

	UFUNCTION(Client, Reliable)
	void S2C_SendMessage(const FText& Message, const FString& SenderName);
	void S2C_SendMessage_Implementation(const FText& Message, const FString& SenderName);
	
	UFUNCTION(Server, Reliable)
	void ServerShowSecondPlayer(bool Ready, const FString& ClientNickname);

	UFUNCTION(Client, Reliable)
	void ClientShowFirstPlayer(bool Ready, const FString& HostNickname);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void StartAfterTimer();

	UFUNCTION(Client, Reliable)
	void ClientSetTimerVisibility();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void DeactiveButton();

	UFUNCTION(Client, Reliable)
	void ClientDeactivateButton();

	UFUNCTION(Server, Reliable)
	void ServerChangeReadyButton(bool Ready);
	
	UFUNCTION(Server, Reliable)
	void ServerShowClientReadyCanvas(bool Show);

	UFUNCTION(Server, Reliable)
	void ClientChangeReadyButton(bool Ready);

	UFUNCTION(Server, Reliable)
	void ServerLeaveRoom();

	UFUNCTION(Client, Reliable)
	void ClientLeaveRoom();

	ULobbyWidgetBase* GetLobbyWidget() const;

	UFUNCTION(Client, Reliable)
	void ClientShowLoading();  // 클라이언트에게 로딩 패널을 보여주는 함수

	void ChangeLobbyCharacterAnim(bool Ready);

	void ShowLoading();

	UFUNCTION(Server, Reliable)
	void ServerRPCTravelTitle();

#pragma region Option

private:
	UFUNCTION()
	void BackOrSystemMenu();


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> SystemMenuIMC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> BackAction;

#pragma endregion Option

#pragma region SystemMenu

	UFUNCTION(Server, Reliable)
	void ServerRPCFriendExitLobbyOrGame();

	UFUNCTION(Client, Reliable)
	void ClientRPCFriendExitLobbyOrGame();


#pragma endregion SystemMenu

public:
	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite)
	TSoftObjectPtr<class ACameraActor> LobbyCamera;

private:

	// todo 채원 경로 바꾸기
	UAnimSequence* ReadyAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/char/anim_retarget_pelvis/Room_Ready.Room_Ready'"));
	UAnimSequence* NotReadyAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/char/anim_retarget_pelvis/Room_Not_Ready.Room_Not_Ready'"));

};
