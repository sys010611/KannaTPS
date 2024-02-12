// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Halo.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

// Sets default values
AHalo::AHalo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollection->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AHalo::BeginPlay()
{
	Super::BeginPlay();
	
	KannaCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	TargetLocation = KannaCharacter->GetMesh()->GetSocketLocation(FName("Halo_Socket"));
	TargetRotation = KannaCharacter->GetMesh()->GetSocketRotation(FName("Halo_Socket"));

	SetActorLocationAndRotation(TargetLocation, TargetRotation);
}

void AHalo::BreakHalo_Implementation()
{
	GeometryCollection->SetSimulatePhysics(true);
	GeometryCollection->SetEnableGravity(true);
}

// Called every frame
void AHalo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TargetLocation = KannaCharacter->GetMesh()->GetSocketLocation(FName("Halo_Socket"));
	TargetRotation = KannaCharacter->GetMesh()->GetSocketRotation(FName("Halo_Socket"));

	SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, 12.5f));
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 12.5f));
}

