
#include "Ping/IndicatorComponent.h"
#include "Widget/PingIndicatorHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/TrapperPlayerController.h"
#include "Kismet/KismetMathLibrary.h"

#include "Widget/PlayerHUD.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"

UIndicatorComponent::UIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 컨트롤러 저장
	PlayerController = Cast<ATrapperPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(PlayerController)
	{
		ControlledPawn = PlayerController->GetPawn();
	}

	// HUD 생성
	if(IsValid(IndicatorHUDClass))
	{
		IndicatorHUDRef = CreateWidget<UPingIndicatorHUD>(GetOwner()->GetGameInstance(), IndicatorHUDClass);
		if(IndicatorHUDRef)
		{
			if(PlayerController && PlayerController->PlayerHudRef)
			{
				PlayerController->PlayerHudRef->IndicatorPanel->AddChild(IndicatorHUDRef);
				IndicatorHUDRef->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				GetWorld()->GetTimerManager().SetTimer(
					TryAddTimer, this, &UIndicatorComponent::TryAddToPlayerHUD, 0.01f, true);
			}
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		UpdateTimer, this, &UIndicatorComponent::UpdateIndicator, 0.01f, true);
}


void UIndicatorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	if(IsValid(IndicatorHUDRef))
	{
		IndicatorHUDRef->RemoveFromParent();
		IndicatorHUDRef = nullptr;
	}
}


void UIndicatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UIndicatorComponent::SetType(EPlayerPingType type)
{
	Type = type;
	if(IndicatorHUDRef)
	{
		IndicatorHUDRef->SetWidgetImage(type);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(
			TEXT("UIndicatorComponent::SetType - HudRef 없음")));
	}
}


void UIndicatorComponent::UpdateIndicator()
{
	if(!IsValid(IndicatorHUDRef) || !IsValid(PlayerController) || !IsValid(IndicatorHUDRef->PanelSlot))
	{
		return;
	}

	/// 거리 업데이트 ---------------------------------------------------------------------------------
	if(ControlledPawn)
	{
		uint32 Dist = static_cast<uint32>(GetOwner()->GetDistanceTo(ControlledPawn) * 0.01);
		FString String = FString::FromInt(Dist) + TEXT("m");
		FText Text = FText::FromString(String);

		if(IsValid(IndicatorHUDRef))
		{
			IndicatorHUDRef->Distance->SetText(/*FText::AsNumber(Dist)*/Text);
		}
	}

	/// 위치 업데이트 ---------------------------------------------------------------------------------

	// 스크린상의 마커 좌표를 구한다
	FVector2D ScreenLocation;
	bool bOnScreen = PlayerController->ProjectWorldLocationToScreen(GetOwner()->GetActorLocation(), ScreenLocation);
	float scale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());

	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
	FVector2D ViewportSize = FVector2D(ViewportSizeX, ViewportSizeY);

	FVector2D CenterOffset = FVector2D(0, 20);
	FVector2D EllipseCenter = FVector2D(ViewportSizeX * 0.5 + CenterOffset.X,
										ViewportSizeY * 0.5 + CenterOffset.Y);

	float RadiusX = ViewportSizeX * 0.677 * 0.5;
	float RadiusY = ViewportSizeY * 0.7 * 0.5;

	if(bOnScreen)
	{
		// 스크린 좌표가 타원 내에 있는지 확인
		if(IsPointInEllipse(ViewportSize, ScreenLocation, EllipseCenter, RadiusX, RadiusY))
		{
			IndicatorHUDRef->PanelSlot->SetPosition(ScreenLocation / scale);
			IndicatorHUDRef->Direction->SetVisibility(ESlateVisibility::Collapsed);

			if(Type == EPlayerPingType::E_OtherPlayer1P || Type == EPlayerPingType::E_OtherPlayer2P)
			{
				IndicatorHUDRef->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		// 타원 밖에 있을 때
		else
		{
			IndicatorHUDRef->PanelSlot->SetPosition(FindClosestPointOnEllipse(ViewportSize, ScreenLocation, EllipseCenter, RadiusX, RadiusY) / scale);
			IndicatorHUDRef->Direction->SetVisibility(ESlateVisibility::Visible);

			if(Type == EPlayerPingType::E_OtherPlayer1P || Type == EPlayerPingType::E_OtherPlayer2P)
			{
				IndicatorHUDRef->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	// 스크린 내에 있지 않을 때
	else
	{
		// 카메라 중심의 월드좌표와 오브젝트 좌표로 노말 계산
		FVector CameraCenterWorldLocation, WorldDirection;
		PlayerController->DeprojectScreenPositionToWorld(ViewportSize.X * 0.5, ViewportSize.Y * 0.5, CameraCenterWorldLocation, WorldDirection);
		FVector ObjectDirection = UKismetMathLibrary::GetDirectionUnitVector(CameraCenterWorldLocation, GetOwner()->GetActorLocation());
		FVector MidPointTowardObject = ObjectDirection * 100 /*Accuary*/;

		// 노말값의 카메라 평면에 대한 투영 계산
		FVector CameraForwardVector = PlayerController->PlayerCameraManager->GetActorForwardVector();
		FVector Projection = MidPointTowardObject - FVector::DotProduct(MidPointTowardObject, CameraForwardVector) * CameraForwardVector;
		FVector CalcLocation = CameraCenterWorldLocation + Projection;
		PlayerController->ProjectWorldLocationToScreen(CalcLocation, ScreenLocation);

		IndicatorHUDRef->PanelSlot->SetPosition(FindClosestPointOnEllipse(ViewportSize, ScreenLocation, EllipseCenter, RadiusX, RadiusY) / scale);
	}
}


void UIndicatorComponent::TryAddToPlayerHUD()
{
	if(PlayerController && PlayerController->PlayerHudRef)
	{
		PlayerController->PlayerHudRef->IndicatorPanel->AddChild(IndicatorHUDRef);
		IndicatorHUDRef->SetVisibility(ESlateVisibility::Visible);

		GetWorld()->GetTimerManager().ClearTimer(TryAddTimer);
	}
}

bool UIndicatorComponent::IsPointInEllipse(FVector2D ViewportSize, FVector2D ScreenPosition, FVector2D EllipseCenter, float RadiusX, float RadiusY)
{
	// 스크린 좌표를 스크린 중앙 기준으로 변환
	float RelativeX = ScreenPosition.X - EllipseCenter.X;
	float RelativeY = ScreenPosition.Y - EllipseCenter.Y;

	// 타원 방정식을 사용하여 점이 타원 내에 있는지 확인
	float NormX = RelativeX / RadiusX;
	float NormY = RelativeY / RadiusY;

	return std::pow(NormX, 2) + std::pow(NormY, 2) <= 1.0f;
}


FVector2D UIndicatorComponent::FindClosestPointOnEllipse(FVector2D ViewportSize, const FVector2D& ScreenPosition, FVector2D EllipseCenter, float RadiusX, float RadiusY)
{
// 	// 스크린 좌표를 원점 기준으로 변환
// 	FVector2D RelativePosition = ScreenPosition - EllipseCenter;
// 
// 	// 중심에서 점까지의 벡터를 정규화
// 	FVector2D NormalizedVector = RelativePosition.GetSafeNormal();
// 
// 	// 타원 위의 점을 찾기 위해 반지름으로 스케일 조정
// 	FVector2D ClosestPoint = FVector2D(
// 		NormalizedVector.X * RadiusX,
// 		NormalizedVector.Y * RadiusY
// 	);
// 
// 	// 원점 기준 좌표를 다시 스크린 좌표로 변환
// 	ClosestPoint += EllipseCenter;

// 스크린 좌표를 원점 기준으로 변환
	FVector2D RelativePosition = ScreenPosition - EllipseCenter;

	// 타원의 각 축에 맞춰 상대 위치를 스케일 조정
	FVector2D ScaledRelativePosition = FVector2D(
		RelativePosition.X / RadiusX,
		RelativePosition.Y / RadiusY
	);

	// 정규화된 벡터를 구한 후 타원의 반지름을 곱해 원래 크기로 조정
	FVector2D NormalizedVector = ScaledRelativePosition.GetSafeNormal();
	FVector2D ClosestPoint = FVector2D(
		NormalizedVector.X * RadiusX,
		NormalizedVector.Y * RadiusY
	);

	// 원점 기준 좌표를 다시 스크린 좌표로 변환
	ClosestPoint += EllipseCenter;

	// 방향 Indicator를 회전시킴
	float Angle = FMath::Atan2(NormalizedVector.Y, NormalizedVector.X);
	Angle = FMath::RadiansToDegrees(Angle);
	IndicatorHUDRef->Direction->SetRenderTransformAngle(Angle);


	// 플레이어 아이콘의 경우 거리에 따라 투명도를 변화시킴
	if(Type == EPlayerPingType::E_OtherPlayer1P || Type == EPlayerPingType::E_OtherPlayer2P)
	{
		float Distance = FVector2D::Distance(ScreenPosition, ClosestPoint);
		float MaxDistance = ViewportSize.X * 0.2;
		float Alpha = Distance / MaxDistance;

		IndicatorHUDRef->SetRenderOpacity(Alpha);

		//UE_LOG(LogTemp, Warning, TEXT("ScreenPos : %s        Closest : %s"), *ScreenPosition.ToString(), *ClosestPoint.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Dist : %f    Max : %f     Alpha : %f"), Distance, MaxDistance, Alpha);
	}

	return ClosestPoint;
}
