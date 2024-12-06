// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyPS.h"
#include "Net/UnrealNetwork.h"
#include "LobbyPC.h"
#include "LobbyWidgetBase.h"

ALobbyPS::ALobbyPS()
{
	bReady = false;
}

void ALobbyPS::SetReady(bool NewReady)
{
    if (!HasAuthority())
    {
        bReady = NewReady;
    }
}