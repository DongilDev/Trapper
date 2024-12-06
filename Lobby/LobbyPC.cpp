// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPC.h"
#include "LobbyWidgetBase.h"
#include "TrapperGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"
#include "LobbyPS.h"
#include "LobbyGM.h"
#include "EnhancedInputSubSystems.h"
#include "EnhancedInputComponent.h"
#include "Widget/SystemMenuUMG.h"
#include "Camera/CameraActor.h"
#include "Animation/SkeletalMeshActor.h"

#include "MediaPlayer.h"
#include "MediaSource.h"
#include "MediaTexture.h"

void ALobbyPC::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;

	// 레벨에서 모든 스켈레탈 메쉬 액터 찾기
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASkeletalMeshActor::StaticClass(), FoundActors);

	// 첫 번째 스켈레탈 메쉬 액터를 SkeletalMesh1 변수로 설정
	if (FoundActors.Num() > 0)
	{
		ASkeletalMeshActor* FoundSkeletalMeshActor1 = Cast<ASkeletalMeshActor>(FoundActors[0]);
		if (FoundSkeletalMeshActor1)
		{
			SkeletalMesh1 = FoundSkeletalMeshActor1->GetSkeletalMeshComponent();
		}

		ASkeletalMeshActor* FoundSkeletalMeshActor2 = Cast<ASkeletalMeshActor>(FoundActors[1]);
		if (FoundSkeletalMeshActor2)
		{
			SkeletalMesh2 = FoundSkeletalMeshActor2->GetSkeletalMeshComponent();
		}
	}

	if (SkeletalMesh1)
	{
		SkeletalMesh1->SetVisibility(true);
	}

	if (IsLocalController() && LobbyWidgetClass)
	{
		LobbyWidgetObject = CreateWidget<ULobbyWidgetBase>(this, LobbyWidgetClass);

		LobbyWidgetObject->AddToViewport();

		UTrapperGameInstance* GI = Cast<UTrapperGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (!HasAuthority())
		{
			// 닉네임 띄우기
			// 클라
			LobbyWidgetObject->ShowSecondPlayerNickname(true, GI->GetPlayerNickname());
			// 서버
			ServerShowSecondPlayer(true, GI->GetPlayerNickname());
			ServerShowClientReadyCanvas(true);
			// 친구초대버튼 없애기
			LobbyWidgetObject->ShowInvitationFriendButton(false);

			if (SkeletalMesh2)
			{
				SkeletalMesh2->SetVisibility(true);
			}
		}
		else
		{
			//LobbyWidgetObject->ServerReadyButton();

			if (SkeletalMesh2)
			{
				SkeletalMesh2->SetVisibility(false);
			}
		}

		bShowMouseCursor = true;

		//FInputModeGameAndUI InputMode;

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		SetInputMode(InputMode);
		SetViewTarget(LobbyCamera.Get());
	}

	UEnhancedInputLocalPlayerSubsystem* SubSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (SubSystem && SystemMenuIMC)
	{
		SubSystem->AddMappingContext(SystemMenuIMC, 0);
	}

	/*if (IsLocalController())
	{
		UE_LOG(LogTrap, Warning, TEXT("SetViewTarget"));
		SetViewTarget(LobbyCamera.Get());
		FInputModeUIOnly InputMode;
		SetInputMode(InputMode);
	}*/
}

void ALobbyPC::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* UIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (UIC)
	{
		UIC->BindAction(BackAction, ETriggerEvent::Started, this, &ALobbyPC::BackOrSystemMenu);
	}
}

//Server에서 실행
bool ALobbyPC::C2S_SendMessage_Validate(const FText& Message)
{

	return true;
}

//Server에서 실행
void ALobbyPC::C2S_SendMessage_Implementation(const FText& Message)
{
	FString PlayerName = GetPlayerState<APlayerState>()->GetPlayerName();

	//c++ 11
	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; Iter++)
	{
		ALobbyPC* PC = Cast<ALobbyPC>(*Iter);
		if (PC)
		{
			//Server to Client
			PC->S2C_SendMessage(Message, PlayerName);
		}
	}
}

//Call Server, Execute Client
void ALobbyPC::S2C_SendMessage_Implementation(const FText& Message, const FString& SenderName)
{
	FString LocalPlayerName = GetPlayerState<APlayerState>()->GetPlayerName();

	// 보낸 사람이 자신인지 확인
	if (SenderName == LocalPlayerName)
	{
		// 자신이 보낸 메시지는 하얀색
		LobbyWidgetObject->AddMessage(Message, FLinearColor::White);
	}
	else
	{
		// 다른 플레이어가 보낸 메시지는 초록색
		LobbyWidgetObject->AddMessage(Message, FLinearColor::Green);
	}
}

void ALobbyPC::ServerShowSecondPlayer_Implementation(bool Ready, const FString& ClientNickname)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ALobbyPC* PC = Cast<ALobbyPC>(Iterator->Get());
		if (PC->LobbyWidgetObject)
		{
			if (Ready)
			{
				PC->LobbyWidgetObject->ShowSecondPlayerNickname(true, ClientNickname);
				UTrapperGameInstance* GI = Cast<UTrapperGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
				ClientShowFirstPlayer(true, GI->GetPlayerNickname());
				// 친구 초대버튼, 친구창 없애기
				PC->LobbyWidgetObject->ShowInvitationFriendButton(false);
				PC->LobbyWidgetObject->ShowInvitationFriendList(false);

				if (SkeletalMesh2)
				{
					SkeletalMesh2->PlayAnimation(NotReadyAnim, false);
					SkeletalMesh2->SetVisibility(true);
				}
			}
			else
			{
				PC->LobbyWidgetObject->ShowSecondPlayerNickname(false, ClientNickname);
				PC->LobbyWidgetObject->ShowInvitationFriendButton(true);

				if (SkeletalMesh2)
				{
					SkeletalMesh2->SetVisibility(false);
				}
			}
		}
	}
}

void ALobbyPC::ClientShowFirstPlayer_Implementation(bool Ready, const FString& HostNickname)
{
	if (LobbyWidgetObject)
	{
		LobbyWidgetObject->ShowFirstPlayerNickname(Ready, HostNickname);
		UE_LOG(LogTemp, Log, TEXT("ClientSetWidgetVisibility called, widget visibility set."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetObject is null in ClientSetWidgetVisibility."));
	}
}

void ALobbyPC::StartAfterTimer()
{
	if (HasAuthority())
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			ALobbyPC* PC = Cast<ALobbyPC>(Iterator->Get());
			if (PC)
			{
				PC->ClientSetTimerVisibility();
			}
		}
	}
}

void ALobbyPC::ClientSetTimerVisibility_Implementation()
{
	if (LobbyWidgetObject)
	{
		LobbyWidgetObject->ShowCountDown();
		UE_LOG(LogTemp, Log, TEXT("ClientSetWidgetVisibility called, widget visibility set."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetObject is null in ClientSetWidgetVisibility."));
	}
}

void ALobbyPC::ServerChangeReadyButton_Implementation(bool Ready)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ALobbyPC* PC = Cast<ALobbyPC>(Iterator->Get());
		if (PC)
		{
			if (HasAuthority() && PC->LobbyWidgetObject)
			{
				if (Ready)
				{
					if (SkeletalMesh2)
					{
						SkeletalMesh2->PlayAnimation(ReadyAnim, false);
					}

					PC->LobbyWidgetObject->ActiveStartButton(true);
				}
				else
				{
					if (SkeletalMesh2)
					{
						SkeletalMesh2->PlayAnimation(NotReadyAnim, false);
					}

					PC->LobbyWidgetObject->ActiveStartButton(false);
				}
			}
		}
	}
}

void ALobbyPC::ClientChangeReadyButton_Implementation(bool Ready)
{
	if (LobbyWidgetObject)
	{
		LobbyWidgetObject->UpdateReadyButton(Ready);
		UE_LOG(LogTemp, Log, TEXT("ClientChangeButtonText called, widget text set."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetObject or PublicMessage is null in ClientChangeButtonText."));
	}
}

void ALobbyPC::ServerLeaveRoom_Implementation()
{
	ALobbyGM* GameMode = Cast<ALobbyGM>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnServerPlayerTitleTravle();
	}

	if (IsLocalController())
	{
		uint8 PlayerNum = GetWorld()->GetNumPlayerControllers();
		if (PlayerNum == 1)
		{
			UGameplayStatics::OpenLevel(this, "Title");
			return;
		}

		// 모든 플레이어 컨트롤러를 순회하면서 타이틀 레벨로 이동
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			ALobbyPC* LobbyPC = Cast<ALobbyPC>(PC);
			if (LobbyPC && LobbyPC != this) // 서버의 PC는 나중에 처리하기 위해 제외
			{
				LobbyPC->ClientTravel("Title", TRAVEL_Absolute);
			}
		}
	}
	else
	{
		ClientTravel("Title", TRAVEL_Absolute);
	}

	// 서버의 PC도 타이틀 레벨로 이동
	//UGameplayStatics::OpenLevel(this, "Title");
}

void ALobbyPC::ClientLeaveRoom_Implementation()
{
	ServerShowSecondPlayer(false, "");
	ServerShowClientReadyCanvas(false);
	UGameplayStatics::OpenLevel(this, "Title");
}

void ALobbyPC::DeactiveButton()
{
	if (HasAuthority())
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			ALobbyPC* PC = Cast<ALobbyPC>(Iterator->Get());
			if (PC)
			{
				PC->ClientDeactivateButton();
			}
		}
	}
}

void ALobbyPC::ServerShowClientReadyCanvas_Implementation(bool Show)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ALobbyPC* PC = Cast<ALobbyPC>(Iterator->Get());
		if (PC)
		{
			if (HasAuthority() && PC->LobbyWidgetObject)
			{
				if (Show)
				{
					PC->LobbyWidgetObject->ShowClientReadyCanvas(true);
				}
				else
				{
					PC->LobbyWidgetObject->ShowClientReadyCanvas(false);
				}
			}
		}
	}
}

void ALobbyPC::ClientDeactivateButton_Implementation()
{
	if (HasAuthority())
	{
		return;
	}

	if (LobbyWidgetObject)
	{
		LobbyWidgetObject->DuringCountDown();
		UE_LOG(LogTemp, Log, TEXT("ClientChangeButtonText called, widget text set."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetObject or PublicMessage is null in ClientChangeButtonText."));
	}
}

ULobbyWidgetBase* ALobbyPC::GetLobbyWidget() const
{
	return LobbyWidgetObject;
}

void ALobbyPC::BackOrSystemMenu()
{
	LobbyWidgetObject->OnBack();
}

void ALobbyPC::ChangeLobbyCharacterAnim(bool Ready)
{
	if (Ready)
	{
		if (SkeletalMesh2)
		{
			SkeletalMesh2->PlayAnimation(ReadyAnim, false);
		}
	}
	else
	{
		if (SkeletalMesh2)
		{
			SkeletalMesh2->PlayAnimation(NotReadyAnim, false);
		}
	}
}

void ALobbyPC::ServerRPCFriendExitLobbyOrGame_Implementation()
{
	UTrapperGameInstance* TrapperGameInstance = Cast<UTrapperGameInstance>(GetGameInstance());
	if (TrapperGameInstance)
	{
		TrapperGameInstance->SetFriendExitLobbyOrGame(true);
		TrapperGameInstance->SetExitFriendName(TEXT("디아나"));
	}
}

void ALobbyPC::ClientRPCFriendExitLobbyOrGame_Implementation()
{
	UTrapperGameInstance* TrapperGameInstance = Cast<UTrapperGameInstance>(GetGameInstance());
	if (TrapperGameInstance)
	{
		TrapperGameInstance->SetFriendExitLobbyOrGame(true);
		TrapperGameInstance->SetExitFriendName(TEXT("리아나"));
	}
}

void ALobbyPC::ClientShowLoading_Implementation()
{
	// 클라이언트 측에서 로딩 패널을 띄우는 로직
	ShowLoading();
}

void ALobbyPC::ShowLoading()
{
	if (LobbyWidgetObject)
	{
		LobbyWidgetObject->ShowLoading();
	}
}

void ALobbyPC::ServerRPCTravelTitle_Implementation()
{
	// 방 없애기
	Protocol::C_DESTROY_ROOM Msg;
	Msg.set_roomnum(Cast<UTrapperGameInstance>(GetGameInstance())->GetRoomNumber());
	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
	Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);

	ALobbyGM* GameMode = Cast<ALobbyGM>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnServerPlayerTitleTravle();
	}

	if (IsLocalController())
	{
		uint8 PlayerNum = GetWorld()->GetNumPlayerControllers();
		if (PlayerNum == 1)
		{
			UGameplayStatics::OpenLevel(this, "Title");
			return;
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			ALobbyPC* LobbyPC = Cast<ALobbyPC>(PC);
			if (LobbyPC && LobbyPC != this)
			{
				LobbyPC->ClientTravel("Title", TRAVEL_Absolute);
				// gameinstance에 있는 방정보 초기화
				Cast<UTrapperGameInstance>(LobbyPC->GetGameInstance())->SetRoomNumber(0);
				Cast<UTrapperGameInstance>(LobbyPC->GetGameInstance())->SetRoomIp("");
			}
		}
	}
	else
	{
		ClientTravel("Title", TRAVEL_Absolute);
		Cast<UTrapperGameInstance>(GetGameInstance())->SetRoomNumber(0);
		Cast<UTrapperGameInstance>(GetGameInstance())->SetRoomIp("");
	}
}
