// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeadbodyParts.generated.h"

UCLASS()
class TRAPPERPROJECT_API ADeadbodyParts : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeadbodyParts();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:
	UFUNCTION(BlueprintCallable, Category = "Custom")
	void PlayParticle();

public:
	UPROPERTY(EditAnywhere, Category = "DeadbodyParts")
	TObjectPtr<class UStaticMeshComponent> DeadbodyPart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara")
	TObjectPtr<class UNiagaraSystem> DissolveParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Niagara")
	class UNiagaraComponent* DissolveParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara")
	TObjectPtr<class USceneComponent> DissolveParticlePosition;

};
