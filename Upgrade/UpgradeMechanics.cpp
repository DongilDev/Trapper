// Fill out your copyright notice in the Description page of Project Settings.


#include "Upgrade/UpgradeMechanics.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"

#include "Character/TrapperPlayer.h"
#include "Framework/TrapperPlayerController.h"
#include "Framework/TrapperGameState.h"
#include "Components/AudioComponent.h"

#include "Widget/UpgradeHUD.h"
#include "Widget/UpgradePartsHUD.h"
#include "UpgradeTable.h"
#include "TrapperProject.h"
#include "Trap/Component/TrapMechanics.h"

UUpgradeMechanics::UUpgradeMechanics()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UUpgradeMechanics::BeginPlay()
{
	Super::BeginPlay();

	PlayerRef = Cast<ATrapperPlayer>(GetOwner());
	PlayerController = Cast<ATrapperPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	GameState = Cast<ATrapperGameState>(GetWorld()->GetGameState());

	SetupInputComponent();
	LastTime = LimitTime;
}


void UUpgradeMechanics::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UUpgradeMechanics::SetupInputComponent()
{
	AActor* OwnerActor = GetOwner();
	if(!IsValid(OwnerActor))
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(!IsValid(PC))
	{
		return;
	}

	UEnhancedInputComponent* UIC = Cast<UEnhancedInputComponent>(PC->InputComponent);
	if(IsValid(UIC))
	{
		UIC->BindAction(DebugAction, ETriggerEvent::Started, this, &UUpgradeMechanics::EnterUpgradeMode);
		UIC->BindAction(SelectAction, ETriggerEvent::Started, this, &UUpgradeMechanics::ExitUpgradeMode);
		UIC->BindAction(SelectAction, ETriggerEvent::Completed, this, &UUpgradeMechanics::ExitUpgradeModeTutorial);
	}
}

///-----------------------------------------------------------------------------------------
///											일반
///-----------------------------------------------------------------------------------------

void UUpgradeMechanics::EnterUpgradeMode()
{
	if(!PlayerRef->IsLocallyControlled() || bIsOnUpgradeMode)
	{
		return;
	}

	if((PlayerRef->HasAuthority() && GameState->IsUpgradeSelected(true))
		|| (!PlayerRef->HasAuthority() && GameState->IsUpgradeSelected(false))
		)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(TEXT("이미 이번 정비 업그레이드를 완료함")));
		UE_LOG(LogTemp, Warning, TEXT("이미 이번 정비 업그레이드를 완료함"));
		return;
	}

	// 함정 UI가 켜져있으면 끄기
	if(PlayerRef->TrapMechanics->IsTrapSelectMode())
	{
		PlayerRef->TrapMechanics->ReleaseSelectMode();
	}

	// 함정 설치모드이면 끄기
	if(PlayerRef->TrapMechanics->bInstallationMode)
	{
		PlayerRef->TrapMechanics->ReleaseInstallationMode();
	}

	// 화살 Block
	PlayerRef->BlockBow();

	bIsOnUpgradeMode = true;
	LastTime = LimitTime;

	PlayerRef->ShowCursor();
	PlayerRef->SetCursorToCenter();
	PlayerRef->BlockBow();

	GetWorld()->GetTimerManager().SetTimer(CheckHoverTimer, this, &UUpgradeMechanics::CheckOnHovered, 0.03, true);
	GetWorld()->GetTimerManager().SetTimer(LimitTimer, this, &UUpgradeMechanics::DecreaseLastTime, 1, true);
	GetWorld()->GetTimerManager().SetTimer(CanSelectableTimer, this, &UUpgradeMechanics::ChangeSelectable, 1, false);

	HudRef = PlayerController->UpgradeHudRef;
	if(IsValid(HudRef))
	{
		HudRef->PlayPopupAnimation();
		HudRef->SetPillarTextVisibility(false);
		HudRef->SetLastTime(LastTime);
		HudRef->OnUnhovered(LastHoveredType);
		HudRef->SetVisibility(ESlateVisibility::HitTestInvisible);
		HudRef->OnHovered(1);
	}

	// 게임스테이트에서 데이터 받아와서 위젯 설정
	const TArray<FUpgradeTable>& table = GameState->GetUpgradeArray();
	if(PlayerRef->HasAuthority() || GameState->MaintenanceCount == 6)
	{
		HudRef->Parts1->Init(table[0].Name, table[0].Description, table[0].IconTextrue);
		HudRef->Parts2->Init(table[1].Name, table[1].Description, table[1].IconTextrue);
		HudRef->Parts3->Init(table[2].Name, table[2].Description, table[2].IconTextrue);
	}
	else
	{
		HudRef->Parts1->Init(table[3].Name, table[3].Description, table[3].IconTextrue);
		HudRef->Parts2->Init(table[4].Name, table[4].Description, table[4].IconTextrue);
		HudRef->Parts3->Init(table[5].Name, table[5].Description, table[5].IconTextrue);
	}

	if(GameState->MaintenanceCount == 6)
	{
		HudRef->Parts4->Init(table[3].Name, table[3].Description, table[3].IconTextrue);
	}
	else
	{
		const FPillarUpgradeTable PillarData = GameState->GetMagneticPillarData();
		FString Desc = PillarData.Description;
		Desc = Desc.Replace(TEXT("%n"), *FString::FromInt(PillarData.EffectAmount));
		HudRef->Parts4->Init(PillarData.Name, Desc, PillarData.IconTextrue);
	}
}

void UUpgradeMechanics::ExitUpgradeMode()
{
	if(!PlayerRef->IsLocallyControlled() || !bIsOnUpgradeMode ||!bCanSelectable)
	{
		return;
	}

	if(LastHoveredType == 3 && !GameState->IsPillarReactivatedUpgradeSeletable())
	{
		HudRef->SetPillarTextVisibility(true);
		return;
	}

	ATrapperPlayer* TP = Cast<ATrapperPlayer>(GetOwner());
	if (TP)
	{
		TP->SetLookInput(true);
	}

	bIsOnUpgradeMode = false;
	bCanSelectable = false;
	GetWorld()->GetTimerManager().ClearTimer(CheckHoverTimer);

	PlayerRef->HideCursor();
	PlayerRef->UnblockBow();
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(PlayerRef, &ATrapperPlayer::UnblockBow);

	if (HudRef)
	{
		HudRef->PlayExitAnimation();
	}

	// 게임스테이트에 전달
	if(PlayerRef->HasAuthority())
	{
		uint8 Temp = LastHoveredType;
		if(LastHoveredType == 3)
		{
			// 자성기둥 활성화는 자동선택 제외
			if(LastTime == -1)
			{
				Temp = 2;
			}
			else
			{
				Temp = 6;
			}
		}
		GameState->SelectUpgrade(Temp, true);
		GameState->OnQuestExecute.Broadcast(4, true);
	}
	else
	{
		uint8 Temp = LastHoveredType + 3;
		if(LastHoveredType == 3)
		{
			// 자성기둥 활성화는 자동선택 제외
			if(LastTime == -1)
			{
				Temp = 5;
			}
			else
			{
				Temp = 6;
			}
		}
		ServerRPCUpgrade(Temp);
		GameState->OnQuestExecute.Broadcast(4, false);
	}

	if(SelectClickSound && PlayerRef->AudioComponent)
	{
		PlayerRef->AudioComponent->SetSound(SelectClickSound);
		PlayerRef->AudioComponent->Play();
	}

	//HudRef->PlayExitAnimation();
}

void UUpgradeMechanics::CheckOnHovered()
{
	if(GEngine && GEngine->GameViewport)
	{
		// 현재 게임 화면의 크기를 가져옴
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);

		// 화면의 1/3, 1/3 좌표 계산
		float half1 = ViewportSize.X * 0.25;
		float half2 = ViewportSize.X * 0.5;
		float half3 = ViewportSize.X * 0.75;

		// 마우스 현재 좌표를 가져옴
		FVector2D MousePosition;
		GEngine->GameViewport->GetMousePosition(MousePosition);

		uint8 currentHovered = -1;

		if(MousePosition.X < half1)
		{
			currentHovered = 0;
		}
		else if(half1 <= MousePosition.X && MousePosition.X <= half2)
		{
			currentHovered = 1;
		}
		else if(half2 <= MousePosition.X && MousePosition.X <= half3)
		{
			currentHovered = 2;
		}
		else
		{
			currentHovered = 3;
		}

		if(LastHoveredType != currentHovered)
		{
			HudRef->OnHovered(currentHovered);
			HudRef->OnUnhovered(LastHoveredType);

			if(SelectOverSound && PlayerRef)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), SelectOverSound, PlayerRef->GetActorLocation());
			}
		}
		LastHoveredType = currentHovered;
	}
}


void UUpgradeMechanics::DecreaseLastTime()
{
	LastTime -= 1;

	if(LastTime == -1)
	{
		ExitUpgradeMode();
		GetWorld()->GetTimerManager().ClearTimer(LimitTimer);
	}
	else if(IsValid(HudRef))
	{
		HudRef->SetLastTime(LastTime);
	}
}

///-----------------------------------------------------------------------------------------
///										튜토리얼 전용
///-----------------------------------------------------------------------------------------

void UUpgradeMechanics::EnterUpgradeModeTutorial()
{
	if(!PlayerRef->IsLocallyControlled()
		|| bIsOnUpgradeModeTutorial
		|| bTutorialCheck )
	{
		return;
	}

	bIsOnUpgradeModeTutorial = true;
	LastTime = LimitTime;

	GetWorld()->GetTimerManager().SetTimer(CheckHoverTimerTutorial, this, &UUpgradeMechanics::CheckOnHoveredTutorial, 0.03, true);
	
	PlayerRef->ShowCursor();
	PlayerRef->SetCursorToCenter();
	PlayerRef->BlockBow();

	HudRef = PlayerController->UpgradeHudRef;
	if(IsValid(HudRef))
	{
		HudRef->PlayPopupAnimation();
		HudRef->SetLastTime(LastTime);
		HudRef->SetVisibility(ESlateVisibility::HitTestInvisible);
		HudRef->SetTutorialBackgroundVisibility(true);
		HudRef->SetPillarTextVisibility(false);
		HudRef->OnUnhovered(LastHoveredType);
	}
	
	//선택지 1,2,3
	const TArray<FUpgradeTable>& table = GameState->GetUpgradeArray();
	HudRef->Parts1->Init(table[0].Name, table[0].Description, table[0].IconTextrue);
	HudRef->Parts2->Init(table[1].Name, table[1].Description, table[1].IconTextrue);
	HudRef->Parts3->Init(table[2].Name, table[2].Description, table[2].IconTextrue);

	// 자성기둥 업글 선택지
	const FPillarUpgradeTable PillarData = GameState->GetMagneticPillarData();
	FString Desc = PillarData.Description;
	Desc = Desc.Replace(TEXT("%n"), *FString::FromInt(PillarData.EffectAmount));
	HudRef->Parts4->Init(PillarData.Name, Desc, PillarData.IconTextrue);
}

void UUpgradeMechanics::ExitUpgradeModeTutorial()
{
	if(	   !PlayerRef->IsLocallyControlled() 
		|| !bIsOnUpgradeModeTutorial
		|| LastHoveredType != 3
		)
	{
		return;
	}

	bIsOnUpgradeModeTutorial = false;
	GetWorld()->GetTimerManager().ClearTimer(CheckHoverTimerTutorial);

	if(IsValid(HudRef))
	{
		HudRef->PlayExitAnimation();
		HudRef->SetTutorialBackgroundVisibility(false);
	}

	PlayerRef->HideCursor();
	PlayerRef->UnblockBow();

	if(SelectClickSound && PlayerRef->AudioComponent)
	{
		PlayerRef->AudioComponent->SetSound(SelectClickSound);
		PlayerRef->AudioComponent->Play();
	}

	bTutorialCheck = true;

	// 게임스테이트로 전달
	if(PlayerRef->HasAuthority())
	{
		GameState->SelectUpgrade(6, true);
		GameState->OnQuestExecute.Broadcast(4, true);
	}
	else
	{
		ServerRPCUpgrade(6);
		GameState->OnQuestExecute.Broadcast(4, false);
	}
}

void UUpgradeMechanics::UpgradeHUDCollapsed()
{
	HudRef->SetVisibility(ESlateVisibility::Collapsed);
}

void UUpgradeMechanics::CheckOnHoveredTutorial()
{
	if(GEngine && GEngine->GameViewport)
	{
		// 현재 게임 화면의 크기를 가져옴
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);

		// 화면의 1/3, 1/3 좌표 계산
		float half3 = ViewportSize.X * 0.75;

		// 마우스 현재 좌표를 가져옴
		FVector2D MousePosition;
		GEngine->GameViewport->GetMousePosition(MousePosition);

		uint8 currentHovered = 0;

		if(MousePosition.X >= half3)
		{
			currentHovered = 3;
		}

		if(LastHoveredType != currentHovered && currentHovered == 3)
		{
			HudRef->OnHovered(currentHovered);

			if(SelectOverSound && PlayerRef)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), SelectOverSound, PlayerRef->GetActorLocation());
			}
		}
		else if(LastHoveredType != currentHovered && currentHovered != 3)
		{
			HudRef->OnUnhovered(3);
		}
		LastHoveredType = currentHovered;
	}
}

void UUpgradeMechanics::ServerRPCUpgrade_Implementation(uint8 HoveredType)
{
	GameState->SelectUpgrade(HoveredType, false);
}

