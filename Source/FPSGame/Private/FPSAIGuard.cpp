// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"

#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "Perception/PawnSensingComponent.h"


// Sets default values
AFPSAIGuard::AFPSAIGuard() : GuardState(EAIState::IDLE)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();
	
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);
}

void AFPSAIGuard::SetGuardState(EAIState new_state)
{
	if (new_state == GuardState)
	{
		return;
	}

	GuardState = new_state;

	OnStateChanged(GuardState);
}

void AFPSAIGuard::ResetRotation()
{
	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::IDLE);
}

void AFPSAIGuard::AlertGuard()
{
	AFPSGameMode* gm = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (!gm)
	{
		return;
	}

	for (int i = SeenPawns.Num() - 1; i >= 0; --i)
	{
		if (PawnSensingComp->HasLineOfSightTo(SeenPawns[i]))
		{
			gm->CompleteMission(SeenPawns[i], false);
			SetGuardState(EAIState::ALERTED);
		}
		else
		{
			SeenPawns.RemoveAt(i);
		}
	}
}

void AFPSAIGuard::OnPawnSeen(APawn* seen_pawn)
{
	if (!seen_pawn)
	{
		return;
	}
	//DrawDebugSphere(GetWorld(), seen_pawn->GetActorLocation(), 32.0, 12, FColor::Red, false, 10.0f);

	FVector direction = seen_pawn->GetActorLocation() - GetActorLocation();
	direction.Normalize();

	FRotator new_look_at = FRotationMatrix::MakeFromX(direction).Rotator();
	new_look_at.Pitch = 0.0f;
	new_look_at.Roll = 0.0f;
	SetActorRotation(new_look_at);

	GetWorldTimerManager().ClearTimer(ResetRotationTimer);
	GetWorldTimerManager().SetTimer(ResetRotationTimer, this, &AFPSAIGuard::ResetRotation, 3.0f, false);

	if (SeenPawns.Contains(seen_pawn))
	{
		return;
	}

	SeenPawns.Add(seen_pawn);
	GetWorldTimerManager().SetTimer(AlertTimer, this, &AFPSAIGuard::AlertGuard, 1.0f, false);

	if (GuardState != EAIState::ALERTED)
	{
		SetGuardState(EAIState::SUSPICIOUS);
	}
}

void AFPSAIGuard::OnNoiseHeard(APawn* pawn_instigator, const FVector& location, float volume)
{
	//DrawDebugSphere(GetWorld(), location, 32.0, 12, FColor::Blue, false, 10.0f);

	FVector direction = location - GetActorLocation();
	direction.Normalize();

	FRotator new_look_at = FRotationMatrix::MakeFromX(direction).Rotator();
	new_look_at.Pitch = 0.0f;
	new_look_at.Roll = 0.0f;
	SetActorRotation(new_look_at);

	GetWorldTimerManager().ClearTimer(ResetRotationTimer);
	GetWorldTimerManager().SetTimer(ResetRotationTimer, this, &AFPSAIGuard::ResetRotation, 3.0f, false);

	if (GuardState != EAIState::ALERTED)
	{
		SetGuardState(EAIState::SUSPICIOUS);
	}
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}