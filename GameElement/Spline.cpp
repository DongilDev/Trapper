// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElement/Spline.h"
#include "Components/SplineComponent.h"

// Sets default values
ASpline::ASpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = Spline;

    Spline->ClearSplinePoints();
    Spline->AddSplinePoint(FVector(0.0f, 0.0f, 0.0f), ESplineCoordinateSpace::Local, true);
    Spline->AddSplinePoint(FVector(0.0f, 0.0f, 0.0f), ESplineCoordinateSpace::Local, true);

    Spline->SetSplinePointType(0, ESplinePointType::Linear, true);
    Spline->SetSplinePointType(1, ESplinePointType::Linear, true);

    Spline->UpdateSpline();
}

// Called when the game starts or when spawned
void ASpline::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpline::SetSplinePoint(FVector v1, FVector v2)
{
	Spline->SetLocationAtSplinePoint(0, v1, ESplineCoordinateSpace::World);
	Spline->SetLocationAtSplinePoint(1, v2, ESplineCoordinateSpace::World);
	Spline->UpdateSpline();
}

