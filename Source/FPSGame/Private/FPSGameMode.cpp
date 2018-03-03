// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSGameState.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();

	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* instigator_pawn, bool mission_success)
{
	if (instigator_pawn)
	{
		// Change view targets if any valid actor found (I DONT LIKE IT USE EVENT GRAPH INSTEAD)
		if (!SpectatingViewpointClass)
		{
			UE_LOG(LogTemp, Log, TEXT("SpectatingViewpointClass not found! Update GameMode with valid SpectatingViewpointClass."));
			return;
		}

		TArray<AActor*> all_actors;
		UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, all_actors);

		AActor* new_view_target;
		if (all_actors.Num() > 0)
		{
			new_view_target = all_actors[0];
			
			for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
			{
				auto* pc = it->Get();
				if (pc)
				{
					pc->SetViewTargetWithBlend(new_view_target, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
				}
			}
		}
	}

	AFPSGameState* gs = GetGameState<AFPSGameState>();
	if (gs)
	{
		gs->MulticastOnMissionComplete(instigator_pawn, mission_success);
	}
}
