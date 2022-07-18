// Fill out your copyright notice in the Description page of Project Settings.


#include "CppTurret.h"
#include "Kismet/KismetMathLibrary.h"
#include "TurretAnimInterface.h"



// Sets default values
ACppTurret::ACppTurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateAbstractDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	TurretMesh = CreateAbstractDefaultSubobject<USkeletalMeshComponent>("TurretMesh");
	TurretMesh->SetupAttachment(Root);
	Beam = CreateAbstractDefaultSubobject<UStaticMeshComponent>("Beam");
	Beam->SetupAttachment(TurretMesh, TEXT("BeamSocket"));

	Target1 = CreateAbstractDefaultSubobject<USceneComponent>("Target1");
	Target1->SetupAttachment(Root);
	Target2 = CreateAbstractDefaultSubobject<USceneComponent>("Target2");
	Target2->SetupAttachment(Root);

	FollowTarget = CreateAbstractDefaultSubobject<USceneComponent>("FollowTarget");
	FollowTarget->SetupAttachment(Root);

	SetBeamLenght(BeamLenght);
}

// Called when the game starts or when spawned
void ACppTurret::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ACppTurret::ChangeBeamTarget, TargetDelay, true, 3.f);
}


// Called every frame
void ACppTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateLookAtTarget(DeltaTime);
}

void ACppTurret::UpdateLookAtTarget(float DeltaTime)
{
	if (LookAtRotation.Equals(TargetRotation, 1.f)) {
		return;
	}

	LookAtRotation += RotationDelta * RotationRateMultiplier * DeltaTime;

	if (TurretMesh->GetAnimInstance()->Implements<UTurretAnimInterface>()) {
		ITurretAnimInterface::Execute_UpdateLookAtRotation(TurretMesh->GetAnimInstance(), LookAtRotation);
	}
}

void ACppTurret::ChangeBeamTarget()
{
	TimerCount++;
	if (TimerCount%2 == 0) {
		FollowTarget->SetWorldLocation(Target1->GetComponentLocation());
	}
	else {
		FollowTarget->SetWorldLocation(Target2->GetComponentLocation());
	}

	FVector Start = TurretMesh->GetSocketLocation("BeamSocket");
	FVector End = FollowTarget->GetComponentLocation();
	TargetRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

	RotationDelta = TargetRotation - LookAtRotation;
	RotationDelta.Normalize();
}

void ACppTurret::SetBeamLenght(float Lenght)
{
	Beam->SetRelativeScale3D(FVector(Lenght/ 5000, Beam->GetRelativeScale3D().Y, Beam->GetRelativeScale3D().Z));
	Beam->SetRelativeLocation(FVector(Lenght/125, 0, 0));
}

