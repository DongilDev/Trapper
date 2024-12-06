#include "SoundManager.h"
#include "Components/AudioComponent.h"

// 기본값 설정
ASoundManager::ASoundManager()
{
	// 모든 프레임에서 Tick()을 호출하도록 이 actor을(를) 설정합니다. 필요하지 않은 경우 이 기능을 해제하여 성능을 향상시킬 수 있습니다.
	PrimaryActorTick.bCanEverTick = true;

	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	RootComponent = Audio;

	Audio->bAllowSpatialization = false;
}

// 게임이 시작되거나 생성될 때 호출됩니다.
void ASoundManager::BeginPlay()
{
	Super::BeginPlay();
}

// 매 프레임마다 호출됨
void ASoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
