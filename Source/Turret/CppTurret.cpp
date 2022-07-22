// Fill out your copyright notice in the Description page of Project Settings.


#include "CppTurret.h"
#include "Kismet/KismetMathLibrary.h"
#include "TurretAnimInterface.h"
#include "CharacterInterface.h"
#include "particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"


#define OUT
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

	P_MuzzleFlash = CreateAbstractDefaultSubobject<UParticleSystemComponent>("MuzzleFlash");
	P_MuzzleFlash->SetupAttachment(TurretMesh, TEXT("BeamSocket"));
	P_MuzzleFlash->SetAutoActivate(false);

	SetBeamLenght(BeamLenght);
}

// Called when the game starts or when spawned
void ACppTurret::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(ChangeTargetTimerHandle, this, &ACppTurret::ChangeBeamTarget, TargetDelay, true, 1.f);
	GetWorldTimerManager().SetTimer(TraceTimerHandle, this, &ACppTurret::TraceBeam, .1f, true, .1f);
}


// Called every frame
void ACppTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Enemy) {
		FollowEnemy(DeltaTime);
	}
	else {
		UpdateLookAtTarget(DeltaTime);
	}
	
	//TraceBeam();
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
	Beam->SetRelativeLocation(FVector(Lenght/39.3, 0, 0));
}

void ACppTurret::TraceBeam()
{
	FHitResult HitResult;
	FVector Start = TurretMesh->GetSocketLocation("BeamSocket");
	FVector End = Start + Beam->GetForwardVector() * BeamLenght;

	FCollisionQueryParams CollQueryParams;
	CollQueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		OUT HitResult, 
		Start, 
		End, 
		ECollisionChannel::ECC_Camera,
		CollQueryParams
	);

	if (bHit) {
		SetBeamLenght(HitResult.Distance+230);
		CheckEnemy(HitResult.GetActor());
	}
	else {
		SetBeamLenght(BeamLenght);
	}
}

void ACppTurret::CheckEnemy(AActor* HitActor)
{
	if (HitActor->Implements<UCharacterInterface>()) {
		bool bEnemy = ICharacterInterface::Execute_IsEnemy(HitActor);
		if (bEnemy) {
			if (!Enemy) {
				Enemy = HitActor;
				//start shooting
				GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &ACppTurret::Shoot, .4f, true, .4f);
				//UE_LOG(LogTemp, Warning, TEXT("EnemyDetected"));
			}
			
		}
	}
	else {
		if (Enemy) {
			Enemy = nullptr;
			//stop shooting
			GetWorldTimerManager().ClearTimer(ShootTimerHandle);
		}
		Enemy = nullptr;
	}
}

void ACppTurret::FollowEnemy(float DeltaTime)
{
	FVector Start = TurretMesh->GetSocketLocation("BeamSocket");
	FVector End = Enemy->GetActorLocation();

	FRotator RotToEnemy = UKismetMathLibrary::FindLookAtRotation(Start, End);


	LookAtRotation = FMath::RInterpTo(LookAtRotation, RotToEnemy, DeltaTime, 10);

	if (TurretMesh->GetAnimInstance()->Implements<UTurretAnimInterface>()) {
		ITurretAnimInterface::Execute_UpdateLookAtRotation(TurretMesh->GetAnimInstance(), LookAtRotation);
	}
}

void ACppTurret::Shoot()
{
	UGameplayStatics::PlaySoundAtLocation(this, ShootSound, P_MuzzleFlash->GetComponentLocation());
	P_MuzzleFlash->Activate(true);
}

