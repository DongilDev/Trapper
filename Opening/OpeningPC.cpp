// Fill out your copyright notice in the Description page of Project Settings.


#include "Opening/OpeningPC.h"
#include "Opening/OpeningGM.h"
#include "Opening/OpeningWidgetBase.h"

#include "EnhancedInputSubSystems.h"
#include "EnhancedInputComponent.h"

void AOpeningPC::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && OpeningWidgetClass)
	{
		OpeningWidgetObject = CreateWidget<UOpeningWidgetBase>(this, OpeningWidgetClass);
		OpeningWidgetObject->AddToViewport();

		UEnhancedInputLocalPlayerSubsystem* SubSystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (SubSystem && OpeningIMC)
		{
			SubSystem->AddMappingContext(OpeningIMC, 0);
		}

		ReportLoadingComplete();
	}
}

void AOpeningPC::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* UIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (UIC)
	{
		UIC->BindAction(SkipAction, ETriggerEvent::Started, this, &AOpeningPC::SkipOpening);
	}
}

void AOpeningPC::ServerClientPressSkip_Implementation()
{
	AOpeningGM* GameMode = Cast<AOpeningGM>(GetWorld()->GetAuthGameMode());
	GameMode->PressedSkip(2);
}

void AOpeningPC::ReportLoadingComplete_Implementation()
{
	if (HasAuthority())
	{
		// GameMode에 로딩 완료 상태를 알림
		AOpeningGM* GameMode = GetWorld()->GetAuthGameMode<AOpeningGM>();

		if (GameMode)
		{
			GameMode->OnPlayerLoaded();
		}
	}
}

void AOpeningPC::ClientPlayMedia_Implementation()
{
	OpeningWidgetObject->PlayOpeningMediaWB();
}

void AOpeningPC::PlayOpeningMediaPC()
{
	OpeningWidgetObject->PlayOpeningMediaWB();
}

void AOpeningPC::SkipOpening()
{
	UE_LOG(LogTemp, Warning, TEXT("Opening Skip Pressed"));
	if (HasAuthority())
	{

		AOpeningGM* GameMode = Cast<AOpeningGM>(GetWorld()->GetAuthGameMode());
		GameMode->PressedSkip(1);		
	}
	else
	{
		ServerClientPressSkip();
	}
}

void AOpeningPC::ClientShowLoading_Implementation()
{
	OpeningWidgetObject->ShowLoading();
}