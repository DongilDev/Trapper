// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/TrapSelectHUD.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/TextBlock.h"

#include "Trap/Utillity/TrapHUDParts.h"
#include "Trap/Utillity/TrapActivationMethodParts.h"
#include "Trap/Utillity/TrapCCParts.h"
#include "Trap/Utillity/TrapInstallLocationParts.h"
#include "Trap/Utillity/TrapCount.h"

#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Character/TrapperPlayer.h"

UTrapSelectHUD::UTrapSelectHUD(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	LoadTable();
	LoadGuide();
	ConvertToMap();
}

void UTrapSelectHUD::NativeConstruct()
{
	Super::NativeConstruct();

	InstallDisabledText->SetVisibility(ESlateVisibility::Collapsed);

	if (TrapCountTable)
	{
		TArray<FName> RowNames = TrapCountTable->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			FTrapCount* TrapCount = TrapCountTable->FindRow<FTrapCount>(RowName, FString());
			if (TrapCount)
			{
				TrapCountArray.Add(*TrapCount);
			}
		}

		BarrelRemainingCountText->SetText(FText::FromString(FString::FromInt(TrapCountArray[0].Count)));
		BarrelMaxCountText->SetText(FText::FromString(FString::FromInt(TrapCountArray[0].Count)));
		ATrapperPlayer* TrapperPlayer = Cast<ATrapperPlayer>(GetOwningPlayerPawn());
		if (IsValid(TrapperPlayer))
		{
			TrapperPlayer->SetGunpowderBarrelNum(TrapCountArray[0].Count);
		}
	}
}

void UTrapSelectHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

/// -----------------------------------------------------------------------------------
///										Hovered
/// -----------------------------------------------------------------------------------

void UTrapSelectHUD::OnHovered(ETrapType type, ETrapHUDType bEnabled, bool bShowGuide)
{
	CurrentType = type;

	float Alpha1 = 0.4f;
	float Alpha2 = 0.6f;
	float Alpha3 = 0.8f;
	FLinearColor EnabledColor = FLinearColor(1, 0.86, 0.44, 1);

	SpikePanel->SetRenderScale(FVector2D(0.65, 0.65));
	Bear->SetRenderScale(FVector2D(0.65, 0.65));
	OilBag->SetRenderScale(FVector2D(0.65, 0.65));
	ClapPanel->SetRenderScale(FVector2D(0.65, 0.65));
	Plank->SetRenderScale(FVector2D(0.65, 0.65));
	BarrelPanel->SetRenderScale(FVector2D(0.65, 0.65));
	Guillotine->SetRenderScale(FVector2D(0.65, 0.65));
	Spear->SetRenderScale(FVector2D(0.65, 0.65));

	Spike->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));
	Bear->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));
	OilBag->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));
	Clap->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));
	Plank->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));
	Barrel->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));
	Guillotine->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));
	Spear->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));

	SpikeText->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));
	ClapText->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));
	BarrelRemainingCountText->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));
	BarrelMaxCountText->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));
	BarrelText->SetColorAndOpacity(FLinearColor(1, 1, 1, Alpha1));

	if(bShowGuide)
	{
		GuideOutline->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		GuideOutline->SetVisibility(ESlateVisibility::Collapsed);
	}

	switch (type)
	{
	case ETrapType::Spike:
		Spear->SetOpacity(Alpha3);
		Guillotine->SetOpacity(Alpha2);
		Bear->SetOpacity(Alpha3);
		OilBag->SetOpacity(Alpha2);
		TargetAngle = -135;
		if (bEnabled == ETrapHUDType::E_Disabled)
		{
			Spike->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
			SpikeText->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
		}
		else
		{
			Spike->SetColorAndOpacity(EnabledColor);
			SpikeText->SetColorAndOpacity(FColor::White);
		}
		if (SpikeGuide && TrapMediaPlayer->OpenSource(SpikeGuide) && bShowGuide)
		{
			GuideVideo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TrapMediaPlayer->Play(); // 재생 시작
			TrapMediaPlayer->SetLooping(true);
		}
		break;

	case ETrapType::Bear:
		Spike->SetOpacity(Alpha3);
		SpikeText->SetOpacity(Alpha3);
		Spear->SetOpacity(Alpha2);
		OilBag->SetOpacity(Alpha3);
		Clap->SetOpacity(Alpha2);
		ClapText->SetOpacity(Alpha2);
		TargetAngle = -180;
		if (bEnabled == ETrapHUDType::E_Disabled)
		{
			Bear->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
		}
		else
		{
			Bear->SetColorAndOpacity(EnabledColor);
		}
		if (BearGuide && TrapMediaPlayer->OpenSource(BearGuide) && bShowGuide)
		{
			GuideVideo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TrapMediaPlayer->Play(); // 재생 시작
			TrapMediaPlayer->SetLooping(true);
		}
		break;

	case ETrapType::Plank:
		Barrel->SetOpacity(Alpha3);
		BarrelRemainingCountText->SetOpacity(Alpha3);
		BarrelMaxCountText->SetOpacity(Alpha3);
		BarrelText->SetOpacity(Alpha3);
		Guillotine->SetOpacity(Alpha2);
		Plank->SetOpacity(Alpha3);
		Clap->SetOpacity(Alpha2);
		ClapText->SetOpacity(Alpha2);
		TargetAngle = 45;
		if (bEnabled == ETrapHUDType::E_Disabled)
		{
			Plank->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
		}
		else
		{
			Plank->SetColorAndOpacity(EnabledColor);
		}
		if (PlankGuide && TrapMediaPlayer->OpenSource(PlankGuide) && bShowGuide)
		{
			GuideVideo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TrapMediaPlayer->Play(); // 재생 시작
			TrapMediaPlayer->SetLooping(true);
		}
		break;

	case ETrapType::GuillotinePendulum:
		Spear->SetOpacity(Alpha3);
		Spike->SetOpacity(Alpha2);
		SpikeText->SetOpacity(Alpha2);
		Barrel->SetOpacity(Alpha3);
		BarrelRemainingCountText->SetOpacity(Alpha3);
		BarrelMaxCountText->SetOpacity(Alpha3);
		BarrelText->SetOpacity(Alpha3);
		Plank->SetOpacity(Alpha2);
		TargetAngle = -45;
		if (bEnabled == ETrapHUDType::E_Disabled)
		{
			Guillotine->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
		}
		else
		{
			Guillotine->SetColorAndOpacity(EnabledColor);
		}
		if (GuillotineGuide && TrapMediaPlayer->OpenSource(GuillotineGuide) && bShowGuide)
		{
			GuideVideo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TrapMediaPlayer->Play(); // 재생 시작
			TrapMediaPlayer->SetLooping(true);
		}
		break;

	case ETrapType::OilBag:
		Guillotine->SetOpacity(Alpha3);
		Plank->SetOpacity(Alpha3);
		Spear->SetOpacity(Alpha2);
		Clap->SetOpacity(Alpha2);
		ClapText->SetOpacity(Alpha2);
		TargetAngle = 0;
		if (bEnabled == ETrapHUDType::E_Disabled)
		{
			OilBag->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
		}
		else
		{
			OilBag->SetColorAndOpacity(EnabledColor);
		}
		if (OilbagGuide && TrapMediaPlayer->OpenSource(OilbagGuide) && bShowGuide)
		{
			GuideVideo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TrapMediaPlayer->Play(); // 재생 시작
			TrapMediaPlayer->SetLooping(true);
		}
		break;

	case ETrapType::GunpowderBarrel:
		Bear->SetOpacity(Alpha3);
		Clap->SetOpacity(Alpha3);
		ClapText->SetOpacity(Alpha3);
		Plank->SetOpacity(Alpha2);
		Spike->SetOpacity(Alpha2);
		SpikeText->SetOpacity(Alpha2);
		TargetAngle = 135;

		if (bEnabled == ETrapHUDType::E_Disabled)
		{
			Barrel->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
			BarrelRemainingCountText->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
			BarrelMaxCountText->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
			BarrelText->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
		}
		else
		{
			Barrel->SetColorAndOpacity(EnabledColor);
			BarrelRemainingCountText->SetColorAndOpacity(FColor::White);
			BarrelMaxCountText->SetColorAndOpacity(FColor::White);
			BarrelText->SetColorAndOpacity(FColor::White);
		}
		if (BarrelGuide && TrapMediaPlayer->OpenSource(BarrelGuide) && bShowGuide)
		{
			GuideVideo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TrapMediaPlayer->Play(); // 재생 시작
			TrapMediaPlayer->SetLooping(true);
		}
		break;

	case ETrapType::Clap:
		OilBag->SetOpacity(Alpha3);
		Bear->SetOpacity(Alpha2);
		Plank->SetOpacity(Alpha3);
		Barrel->SetOpacity(Alpha2);
		BarrelRemainingCountText->SetOpacity(Alpha2);
		BarrelMaxCountText->SetOpacity(Alpha2);
		BarrelText->SetOpacity(Alpha2);
		TargetAngle = 90;
		if (bEnabled == ETrapHUDType::E_Disabled)
		{
			Clap->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
			ClapText->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
		}
		else
		{
			Clap->SetColorAndOpacity(EnabledColor);
			ClapText->SetColorAndOpacity(FColor::White);
		}
		if (ClapGuide && TrapMediaPlayer->OpenSource(ClapGuide) && bShowGuide)
		{
			GuideVideo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TrapMediaPlayer->Play(); // 재생 시작
			TrapMediaPlayer->SetLooping(true);
		}
		break;

	case ETrapType::Spear:
		Spike->SetOpacity(Alpha3);
		SpikeText->SetOpacity(Alpha3);
		Bear->SetOpacity(Alpha2);
		Guillotine->SetOpacity(Alpha3);
		Barrel->SetOpacity(Alpha2);
		BarrelRemainingCountText->SetOpacity(Alpha2);
		BarrelMaxCountText->SetOpacity(Alpha2);
		BarrelText->SetOpacity(Alpha2);
		TargetAngle = -90;
		if (bEnabled == ETrapHUDType::E_Disabled)
		{
			Spear->SetColorAndOpacity(FLinearColor(1, 0.1458, 0.1458, 1));
		}
		else
		{
			Spear->SetColorAndOpacity(EnabledColor);
		}
		if (SpearGuide && TrapMediaPlayer->OpenSource(SpearGuide) && bShowGuide)
		{
			GuideVideo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TrapMediaPlayer->Play(); // 재생 시작
			TrapMediaPlayer->SetLooping(true);
		}
		break;

	case ETrapType::None:
		CenterPanel->SetVisibility(ESlateVisibility::Collapsed);
		CenterRing->SetVisibility(ESlateVisibility::Collapsed);
		if (TrapMediaPlayer)
		{
			TrapMediaPlayer->Pause();
			GuideVideo->SetVisibility(ESlateVisibility::Collapsed);
			GuideOutline->SetVisibility(ESlateVisibility::Collapsed);
		}
		break;
	}

	if (type != ETrapType::None)
	{
		CenterPanel->SetVisibility(ESlateVisibility::Visible);
		CenterRing->SetVisibility(ESlateVisibility::Visible);

		TrapName->SetText(FText::FromString(TrapData[type].Name));
		Cost->SetText(FText::FromString(TrapCosts[type]));
		Damage->SetText(FText::FromString(TrapDamages[type]));

		InstallPosImage->SetBrushFromTexture(LocationData[TrapData[type].LocationType].Image);
		InstallPosText->SetText(FText::FromString(LocationData[TrapData[type].LocationType].Description));

		ActivateTypeImage->SetBrushFromTexture(ActivationData[TrapData[type].ActivationType].Image);
		ActivateTypeText->SetText(FText::FromString(ActivationData[TrapData[type].ActivationType].Description));

		if (TrapData[type].CCType == ETrapCCType::E_None)
		{
			CCTypeImage->SetVisibility(ESlateVisibility::Collapsed);
			CCTypeText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			CCTypeImage->SetBrushFromTexture(CCData[TrapData[type].CCType].Image);
			CCTypeText->SetText(FText::FromString(CCData[TrapData[type].CCType].Description));

			CCTypeImage->SetVisibility(ESlateVisibility::Visible);
			CCTypeText->SetVisibility(ESlateVisibility::Visible);
		}

		if (bEnabled == ETrapHUDType::E_Enabled)
		{
			Cost->SetColorAndOpacity(FColor::White);
		}
		else
		{
			Cost->SetColorAndOpacity(FColor::Red);
		}
	}
}


void UTrapSelectHUD::LerpAngle(float DeltaTime)
{
	if (CurrentType != ETrapType::None)
	{
		float CurrentAngle = SideRing->GetRenderTransformAngle();
		float Delta = FMath::Fmod(TargetAngle - CurrentAngle + 360.0f, 360.0f);
		if (Delta > 180.0f)	Delta -= 360.0f;
		float Result = CurrentAngle + Delta * DeltaTime * 7;
		SideRing->SetRenderTransformAngle(Result);
		CenterRing->SetRenderTransformAngle(Result);

		float CurrnetScale = 1.f;
		float TargetScale = 1.f;
		float ScaleValue = 10;

		switch (CurrentType)
		{
		case ETrapType::Spike:
			CurrnetScale = SpikePanel->GetRenderTransform().Scale.X;
			Result = FMath::Lerp(CurrnetScale, TargetScale, DeltaTime * ScaleValue);
			SpikePanel->SetRenderScale(FVector2D(Result, Result));
			break;

		case ETrapType::Bear:
			CurrnetScale = Bear->GetRenderTransform().Scale.X;
			Result = FMath::Lerp(CurrnetScale, TargetScale, DeltaTime * ScaleValue);
			Bear->SetRenderScale(FVector2D(Result, Result));
			break;

		case ETrapType::Plank:
			CurrnetScale = Plank->GetRenderTransform().Scale.X;
			Result = FMath::Lerp(CurrnetScale, TargetScale, DeltaTime * ScaleValue);
			Plank->SetRenderScale(FVector2D(Result, Result));
			break;

		case ETrapType::GuillotinePendulum:
			CurrnetScale = Guillotine->GetRenderTransform().Scale.X;
			Result = FMath::Lerp(CurrnetScale, TargetScale, DeltaTime * ScaleValue);
			Guillotine->SetRenderScale(FVector2D(Result, Result));
			break;

		case ETrapType::GunpowderBarrel:
			CurrnetScale = BarrelPanel->GetRenderTransform().Scale.X;
			Result = FMath::Lerp(CurrnetScale, TargetScale, DeltaTime * ScaleValue);
			BarrelPanel->SetRenderScale(FVector2D(Result, Result));
			break;

		case ETrapType::OilBag:
			CurrnetScale = OilBag->GetRenderTransform().Scale.X;
			Result = FMath::Lerp(CurrnetScale, TargetScale, DeltaTime * ScaleValue);
			OilBag->SetRenderScale(FVector2D(Result, Result));
			break;

		case ETrapType::Clap:
			CurrnetScale = ClapPanel->GetRenderTransform().Scale.X;
			Result = FMath::Lerp(CurrnetScale, TargetScale, DeltaTime * ScaleValue);
			ClapPanel->SetRenderScale(FVector2D(Result, Result));
			break;

		case ETrapType::Spear:
			CurrnetScale = Spear->GetRenderTransform().Scale.X;
			Result = FMath::Lerp(CurrnetScale, TargetScale, DeltaTime * ScaleValue);
			Spear->SetRenderScale(FVector2D(Result, Result));
			break;
		}
	}
}

/// -----------------------------------------------------------------------------------
///									Getter, Setter
/// -----------------------------------------------------------------------------------


FVector2D UTrapSelectHUD::GetSize()
{
	return CenterRing->GetCachedGeometry().GetDrawSize();
}

FVector2D UTrapSelectHUD::GetCenterPos()
{
	FGeometry Geometry = CenterRing->GetCachedGeometry();
	FVector2D pixelPos, viewportPos;
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), Geometry, FVector2D(0, 0), pixelPos, viewportPos);

	viewportPos *= UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	viewportPos += GetSize() * 0.5f;

	//UE_LOG(LogTemp, Warning, TEXT("CenterPos : %s"), *viewportPos.ToString());
	return viewportPos;
}

void UTrapSelectHUD::UpgradeCost(float ratio)
{
	for (const TPair<ETrapType, FTrapHUDParts>& Pair : TrapData)
	{
		FString OriginCost = Pair.Value.Cost;
		int32 Integer = FCString::Atoi(*OriginCost);
		Integer *= ratio;
		TrapCosts[Pair.Key] = FString::FromInt(Integer);
	}
}

void UTrapSelectHUD::UpgradeDamage(float ratio)
{
	for (const TPair<ETrapType, FTrapHUDParts>& Pair : TrapData)
	{
		FString OriginDmg = Pair.Value.Damage;
		int32 Integer = FCString::Atoi(*OriginDmg);
		Integer *= ratio;
		TrapDamages[Pair.Key] = FString::FromInt(Integer);
	}
}

/// -----------------------------------------------------------------------------------
///									테이블 로드
/// -----------------------------------------------------------------------------------


void UTrapSelectHUD::LoadTable()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(
		TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_TrapHUD.DT_TrapHUD'"));

	if (DataTableFinder.Succeeded())
	{
		TrapTable = DataTableFinder.Object;
	}

	//
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder2(
		TEXT("/Script/Engine.DataTable'/Game/Blueprints/Widget/TrapHUD/DT_TrapInstallationLocation.DT_TrapInstallationLocation'"));

	if (DataTableFinder2.Succeeded())
	{
		LocationTable = DataTableFinder2.Object;
	}

	//
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder3(
		TEXT("/Script/Engine.DataTable'/Game/Blueprints/Widget/TrapHUD/DT_TrapActivationMethod.DT_TrapActivationMethod'"));

	if (DataTableFinder3.Succeeded())
	{
		ActiveTable = DataTableFinder3.Object;
	}

	//
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder4(
		TEXT("/Script/Engine.DataTable'/Game/Blueprints/Widget/TrapHUD/DT_TrapCC.DT_TrapCC'"));

	if (DataTableFinder4.Succeeded())
	{
		CCTable = DataTableFinder4.Object;
	}
}

void UTrapSelectHUD::LoadGuide()
{
	static ConstructorHelpers::FObjectFinder<UMediaPlayer> TrapMediaPlayerFinder(
		TEXT("/Script/MediaAssets.MediaPlayer'/Game/Movies/TrapGuidePlayer.TrapGuidePlayer'"));
	if(TrapMediaPlayerFinder.Succeeded())
	{
		TrapMediaPlayer = TrapMediaPlayerFinder.Object;
	}

	// UMediaSource를 각 가이드마다 FObjectFinder를 통해 로드
	ConstructorHelpers::FObjectFinder<UMediaSource> BarrelGuideFinder(
		TEXT("/Script/MediaAssets.FileMediaSource'/Game/Movies/Trap_Info/GunpowderBarrelTrap_Info.GunpowderBarrelTrap_Info'"));
	if(BarrelGuideFinder.Succeeded())
	{
		BarrelGuide = BarrelGuideFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<UMediaSource> OilbagGuideFinder(
		TEXT("/Script/MediaAssets.FileMediaSource'/Game/Movies/Trap_Info/OilBagTrap_Info.OilBagTrap_Info'"));
	if(OilbagGuideFinder.Succeeded())
	{
		OilbagGuide = OilbagGuideFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<UMediaSource> SpearGuideFinder(
		TEXT("/Script/MediaAssets.FileMediaSource'/Game/Movies/Trap_Info/SpearTrap_Info.SpearTrap_Info'"));
	if(SpearGuideFinder.Succeeded())
	{
		SpearGuide = SpearGuideFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<UMediaSource> PlankGuideFinder(
		TEXT("/Script/MediaAssets.FileMediaSource'/Game/Movies/Trap_Info/PlankTrap_Info.PlankTrap_Info'"));
	if(PlankGuideFinder.Succeeded())
	{
		PlankGuide = PlankGuideFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<UMediaSource> BearGuideFinder(
		TEXT("/Script/MediaAssets.FileMediaSource'/Game/Movies/Trap_Info/BearTrap_Info.BearTrap_Info'"));
	if(BearGuideFinder.Succeeded())
	{
		BearGuide = BearGuideFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<UMediaSource> SpikeGuideFinder(
		TEXT("/Script/MediaAssets.FileMediaSource'/Game/Movies/Trap_Info/SpikeTrap_Info.SpikeTrap_Info'"));
	if(SpikeGuideFinder.Succeeded())
	{
		SpikeGuide = SpikeGuideFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<UMediaSource> ClapGuideFinder(
		TEXT("/Script/MediaAssets.FileMediaSource'/Game/Movies/Trap_Info/ClapTrap_Info.ClapTrap_Info'"));
	if(ClapGuideFinder.Succeeded())
	{
		ClapGuide = ClapGuideFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<UMediaSource> GuillotineGuideFinder(
		TEXT("/Script/MediaAssets.FileMediaSource'/Game/Movies/Trap_Info/GuillotinePendulumTrap_Info.GuillotinePendulumTrap_Info'"));
	if(GuillotineGuideFinder.Succeeded())
	{
		GuillotineGuide = GuillotineGuideFinder.Object;
	}
}

void UTrapSelectHUD::ConvertToMap()
{
	TArray<FName> RowNames;
	RowNames = TrapTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FTrapHUDParts* Data = TrapTable->FindRow<FTrapHUDParts>(RowName, FString());
		TrapData.Add(Data->Type, *Data);
		TrapCosts.Add(Data->Type, Data->Cost);
		TrapDamages.Add(Data->Type, Data->Damage);
	}


	RowNames = LocationTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FTrapInstallLocationParts* Data = LocationTable->FindRow<FTrapInstallLocationParts>(RowName, FString());
		LocationData.Add(Data->LocationType, *Data);
	}


	RowNames = ActiveTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FTrapActivationMethodParts* Data = ActiveTable->FindRow<FTrapActivationMethodParts>(RowName, FString());
		ActivationData.Add(Data->ActivationType, *Data);
	}


	RowNames = CCTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FTrapCCParts* Data = CCTable->FindRow<FTrapCCParts>(RowName, FString());
		CCData.Add(Data->CCType, *Data);
	}
}
