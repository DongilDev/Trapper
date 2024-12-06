// Fill out your copyright notice in the Description page of Project Settings.


#include "Ping/HitIndicatorComponent.h"
#include "Framework/TrapperPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/PlayerHUD.h"
#include "Widget/DirectionalDamageIndicatorHUD.h"
#include "Components/CanvasPanel.h"
#include "Monster/BaseMonster.h"
#include "Character/TrapperPlayer.h"
#include "Camera/CameraComponent.h"


UHitIndicatorComponent::UHitIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UHitIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<ATrapperPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	Player = Cast<ATrapperPlayer>(GetOwner());
}


void UHitIndicatorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	for(TPair<int32, FHitIndicatorData>& Elem : Indicators)
	{
		if(IsValid(Elem.Value.HUD))
		{
			Elem.Value.HUD-> RemoveFromParent();
		}
	}
}


void UHitIndicatorComponent::OnPlayerHit(class ABaseMonster* Monster)
{
	if(!IsValid(Monster))
	{
		return;
	}

	int32 ID = Monster->GetUniqueID();

	// 같은게 이미 있는지 확인
	if(Indicators.Contains(ID))
	{
		Indicators[ID].LifeTime = 0.f;
		return;
	}

	// HUD 생성
	if(IsValid(IndicatorHUDClass))
	{
		UDirectionalDamageIndicatorHUD* IndicatorHUDRef = CreateWidget<UDirectionalDamageIndicatorHUD>(GetOwner()->GetGameInstance(), IndicatorHUDClass);
		if(IsValid(IndicatorHUDRef) && IsValid(PlayerController) && IsValid(PlayerController->PlayerHudRef))
		{
			PlayerController->PlayerHudRef->HitIndicatorPanel->AddChild(IndicatorHUDRef);
			IndicatorHUDRef->SetVisibility(ESlateVisibility::HitTestInvisible);
			FHitIndicatorData Data{ Monster, IndicatorHUDRef };
			Indicators.Add(ID, Data);
		}
	}

	if(!bIsUpdating)
	{
		GetWorld()->GetTimerManager().SetTimer(
			UpdateTimer, this, &UHitIndicatorComponent::UpdateIndicator, UpdateTime, true);
		bIsUpdating = true;
	}
}

void UHitIndicatorComponent::UpdateIndicator()
{
	// 업데이트
	for(TPair<int32, FHitIndicatorData>& Elem : Indicators)
	{
		if(    !IsValid(Elem.Value.Monster)
			|| Elem.Value.Monster->MonsterState == EMonsterStateType::IsDestroy
			|| !IsValid(Elem.Value.HUD)
			|| 3 <= Elem.Value.LifeTime)
		{
			ForDeletes.Add(Elem.Key);
		}
		else
		{
			//방향 계산
			FVector CameraVector = Player->Camera->GetForwardVector();
			FVector CameraLocation = Player->Camera->GetComponentLocation();
			FVector MonsterLocation = Elem.Value.Monster->GetActorLocation();
			FVector CameraMonsterVector = MonsterLocation - CameraLocation;

			float CameraDegree = FMath::Atan2(CameraVector.Y , CameraVector.X);
			float CameraMonsterVectorDegree = FMath::Atan2(CameraMonsterVector.Y, CameraMonsterVector.X);
			CameraDegree = FMath::RadiansToDegrees(CameraDegree);
			CameraMonsterVectorDegree = FMath::RadiansToDegrees(CameraMonsterVectorDegree);
			
			Elem.Value.LifeTime += UpdateTime;
			Elem.Value.HUD->SetDirection(CameraMonsterVectorDegree - CameraDegree, Elem.Value.LifeTime);
		}
	}
	
	// 불필요한 hud 삭제
	for(int i = 0; i < ForDeletes.Num(); i++)
	{
		Indicators[ForDeletes[i]].HUD->RemoveFromParent();
		Indicators.Remove(ForDeletes[i]);
	}
	ForDeletes.Empty();

	// 배열이 비었다면 타이머 끄기
	if(Indicators.Num() == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimer);
		bIsUpdating = false;
	}
}
