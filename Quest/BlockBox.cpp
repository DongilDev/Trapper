// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/BlockBox.h"
#include "Components/BoxComponent.h"

ABlockBox::ABlockBox()
{
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = Box;
}
