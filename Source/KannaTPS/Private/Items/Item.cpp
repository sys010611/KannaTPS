// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Components/SphereComponent.h"
#include "Character/KannaCharacter.h"
#include "Managers/ConversationManager.h"

// Sets default values
AItem::AItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
}


void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKannaCharacter* KannaCharacter = Cast<AKannaCharacter>(OtherActor);
	if (KannaCharacter)
	{
		UConversationManager* CM = GetGameInstance()->GetSubsystem<UConversationManager>();
		CM->SetConversation(TEXT("칸나"), TEXT("이건.. 선생님이 사용하시던.."));

		FTimerHandle TimerHandle;

		std::function<void(void)> func = [CM](){CM->SetMessage(TEXT("싯딤의 상자를 챙겼다."));};

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, func, 1.f, false);

		this->Destroy();
	}
}
