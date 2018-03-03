// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"
	
class UPawnSensingComponent;
class ATargetPoint;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	IDLE,
	SUSPICIOUS,
	ALERTED
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetGuardState(EAIState new_state);
	ATargetPoint* GetNextNavPoint();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;
	UPROPERTY(EditInstanceOnly, Category = "Paths")
	TArray<ATargetPoint*> PathPoints;
	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EAIState GuardState;

	UFUNCTION()
	void OnRep_GuardState();
	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);
	UFUNCTION()
	void OnNoiseHeard(APawn* pawn_instigator, const FVector& location, float volume);
	UFUNCTION()
	void ResetRotation();
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState new_state);
	UFUNCTION()
	void AlertGuard();

	FRotator OriginalRotation;
	FTimerHandle ResetRotationTimer;
	FTimerHandle AlertTimer;
	TArray<APawn*> SeenPawns;
	int CurrentNavIndex = 0;
	int LastNavIndex = 0;
};
