// Fill out your copyright notice in the Description page of Project Settings.


#include "TitlePC.h"
#include "TitleWidgetBase.h"
#include "FriendWidgetBase.h"
#include "InvitationWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "TrapperGameInstance.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"
#include "EnhancedInputSubSystems.h"
#include "EnhancedInputComponent.h"
#include "Widget/Options/OptionsUMG.h"
#include "Components/Image.h"


ATitlePC::ATitlePC()
{
	static ConstructorHelpers::FObjectFinder<UInputAction> BackActionFinder(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Back.IA_Back'"));
	if (BackActionFinder.Succeeded())
	{
		BackAction = BackActionFinder.Object;
	}
}

void ATitlePC::BeginPlay()
{
	Super::BeginPlay();

	// 서버인지 확인
	if (IsLocalController() && HasAuthority())
	{
		// 서버인 경우, 게임 모드를 종료하고 타이틀 화면을 열도록 처리합니다.
		UE_LOG(LogTemp, Warning, TEXT("Server detected in Title Level. Shutting down server."));
	}

	Cast<UTrapperGameInstance>(GetGameInstance())->ConnectToGameServer();

	if (IsLocalController() && TitleWidgetClass)
	{
		TitleWidgetObject = CreateWidget<UTitleWidgetBase>(this, TitleWidgetClass);
		TitleWidgetObject->AddToViewport();
		bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		SetInputMode(InputMode);
	}

	/*if (UFriendWidgetClass)
	{
		UFriendWidgetObject = CreateWidget<UFriendWidgetBase>(this, UFriendWidgetClass);
		UFriendWidgetObject->AddToViewport();
		UFriendWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	}*/

	UEnhancedInputLocalPlayerSubsystem* SubSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (SubSystem && SystemMenuIMC)
	{
		SubSystem->AddMappingContext(SystemMenuIMC, 0);
	}
}

void ATitlePC::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* UIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (UIC)
	{
		UIC->BindAction(BackAction, ETriggerEvent::Started, this, &ATitlePC::OnBackButton);
	}
}

void ATitlePC::ShowInputRoomNumberPopup(bool Show)
{
	// 방번호 입력 팝업 뜨게할지 안 뜨게할지
	TitleWidgetObject->ShowInputRoomNumberPopUp(Show);
}

void ATitlePC::ShowFriendPopUp(bool Show)
{
	TitleWidgetObject->ShowFriendPopUp(Show);
}

void ATitlePC::SetClickedButton()
{
	TitleWidgetObject->SetClickedButton();
}

void ATitlePC::ShowRoomErrorText(int32 RoomNum)
{
	TitleWidgetObject->ShowRoomError(RoomNum);
}

void ATitlePC::ShowAddFriend(TArray<int32> SuccessArr, TArray<FString> NicknameArr)
{
	TitleWidgetObject->ShowAddFriend(SuccessArr, NicknameArr);
}

void ATitlePC::ShowInvitationList(int32 RoomNumber, FString Nickname)
{
	TitleWidgetObject->AddInvitation(RoomNumber, Nickname);
}

void ATitlePC::ShowRoomList(int32 RoomNumber, FString Nickname)
{
	TitleWidgetObject->AddRoomList(RoomNumber, Nickname);
}

void ATitlePC::ShowRoomNotExist(bool Show)
{
	TitleWidgetObject->ShowRoomNotExist(Show);
}

void ATitlePC::OnBackButton()
{
	TitleWidgetObject->OnBack();
}
