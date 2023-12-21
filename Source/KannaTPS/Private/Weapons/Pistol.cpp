// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Pistol.h"
#include "Kismet/GameplayStatics.h"
#include "Character/KannaCharacter.h"
#include "Character/Enemy.h"

// Sets default values
APistol::APistol()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FireMode = EFireMode::EFM_SEMIAUTO;

	Range = 10000.f;
}

// Called when the game starts or when spawned
void APistol::BeginPlay()
{
	Super::BeginPlay();
	
	AKannaCharacter* KannaCharacter = Cast<AKannaCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (KannaCharacter)
	{
		KannaCharacter->AddWeaponToList(this); //권총은 시작할 때부터 가지고 있어야 한다.

		this->AttachToComponent(KannaCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Pistol_Socket"));
	}
}


// Called every frame
void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APistol::Fire(FVector& StartPoint, FVector& Direction)
{
	UE_LOG(LogTemp, Warning, TEXT("PISTOL FIRE")); //로그에 출력

	FHitResult HitResult;
	FVector EndPoint = StartPoint + Direction * Range;

	if (GetWorld())
	{
		DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Red, true, -1.f, 0, 2.f);

		if (GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartPoint,
			EndPoint,
			ECollisionChannel::ECC_Visibility))
		{
			if (HitResult.GetActor())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s"), *(HitResult.GetActor())->GetName());
			}
		}
	}
}

