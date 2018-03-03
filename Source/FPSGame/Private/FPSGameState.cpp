// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameState.h"
#include "FPSPlayerController.h"


void AFPSGameState::MulticastOnMissionComplete_Implementation(APawn* instigator_pawn, bool mission_success)
{
	for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
	{
		AFPSPlayerController* pc = Cast<AFPSPlayerController>(it->Get());
		if (pc && pc->IsLocalController())
		{
			pc->OnMissionCompleted(instigator_pawn, mission_success);
		}
	}
}

