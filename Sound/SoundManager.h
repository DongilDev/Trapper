#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundManager.generated.h"

UCLASS()
class TRAPPERPROJECT_API ASoundManager : public AActor
{
	GENERATED_BODY()
	
public:
	// 이 actor의 속성에 대한 기본값을 설정합니다.
	ASoundManager();

protected:
	// 게임이 시작되거나 생성될 때 호출됩니다.
	virtual void BeginPlay() override;

public:
	TObjectPtr<class UAudioComponent> Audio;

public:
	// 매 프레임마다 호출됨
	virtual void Tick(float DeltaTime) override;
};
