#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unarmed UMETA(DisplayName = "Unarmed"),
	ECS_ArmedWithPistol UMETA(DisplayName = "Armed With Pistol"),
	ECS_ArmedWithRifle UMETA(DisplayName = "Armed With Rifle")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Neutral UMETA(DisplayName = "Neutral"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Rolling UMETA(DisplayName = "Rolling"),
	EAS_Aiming UMETA(DisplayName = "Aiming"),
};