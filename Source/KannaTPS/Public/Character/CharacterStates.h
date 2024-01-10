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

UENUM(BlueprintType)
enum class EAimingDirection : uint8
{
	EAD_Neutral UMETA(DisplayName = "Neutral"),
	EAD_Left UMETA(DisplayName = "Left"),
	EAD_Right UMETA(DisplayName = "Right")
};

//UENUM(BlueprintType)
//enum class EEnemyState : uint8
//{
//	EES_Patrol UMETA(DisplayName = "Patrol"),
//	EES_Chase UMETA(DisplayName = "Chase"),
//	EES_Hide UMETA(DisplayName = "Hide"),
//	EES_Attack UMETA(DisplayName = "Attack"),
//};