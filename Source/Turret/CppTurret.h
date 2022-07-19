// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CppTurret.generated.h"

UCLASS()
class TURRET_API ACppTurret : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* TurretMesh;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Beam;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* Target1;
	UPROPERTY(VisibleAnywhere)
		USceneComponent* Target2;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* FollowTarget;

	UPROPERTY()
		int TimerCount = 0;

	UPROPERTY()
		FTimerHandle ChangeTargetTimerHandle;

	UPROPERTY()
		FTimerHandle TraceTimerHandle;

	FRotator LookAtRotation;
	FRotator TargetRotation;
	FRotator RotationDelta;

	UPROPERTY(EditAnywhere)
		float TargetDelay = 3.f;

	UPROPERTY(EditAnywhere)
		float RotationRateMultiplier = 1.f;

	UPROPERTY()
		AActor* Enemy = nullptr;

	UFUNCTION()
		void UpdateLookAtTarget(float DeltaTime);

	UFUNCTION()
		void ChangeBeamTarget();

	UFUNCTION(BlueprintCallable)
		void SetBeamLenght(float Lenght);

	UFUNCTION()
		void TraceBeam();

	UFUNCTION()
		void CheckEnemy(AActor* HitActor);

	UFUNCTION()
		void FollowEnemy(float DeltaTime);

public:	
	// Sets default values for this actor's properties
	ACppTurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BeamLenght = 4000.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
