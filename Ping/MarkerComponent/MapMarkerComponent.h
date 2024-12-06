// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapMarkerType.h"
#include "MapMarkerComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRAPPERPROJECT_API UMapMarkerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMapMarkerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetType(EMapObjectType type);
	void SetTrapType(EMapTrapType type);
	void SetOffset(FVector offset);

public:
	UPROPERTY(EditAnywhere)
	EMapObjectType ObjectType = EMapObjectType::None;
	EMapTrapType TrapType = EMapTrapType::None;

private:
	UPROPERTY() class ATrapperPlayer* PlayerRef = nullptr;
	UPROPERTY() class AActor* OwnerActorRef = nullptr;

	bool bIsCreatedMarker = false;
	FVector Offset = FVector();
};
