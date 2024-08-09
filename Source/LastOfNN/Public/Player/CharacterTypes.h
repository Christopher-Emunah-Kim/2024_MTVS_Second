#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Grabbed UMETA(DisplayName = "Grabbed"),
	ECS_Escape UMETA(DisplayName = "Escape"),
	ECS_UnGrabbed UMETA(DisplayName = "UnGrabbed"),
	ECS_Crouching UMETA(DisplayName = "Crouching"),
	ECS_Executing UMETA(DisplayName = "Executing")
};

UENUM(BlueprintType)
enum class ECharacterEquipState : uint8
{
	ECES_UnEquipped UMETA(DisplayName = "UnEquipped"),
	ECES_GunEquipped UMETA(DisplayName = "GunEquipped"),
	ECES_BatEquipped UMETA(DisplayName = "BatEquipped"),
	ECES_ThrowWeaponEquipped UMETA(DisplayName = "ThrowWeaponEquipped"),
	ECES_ShotgunEquipped UMETA(DisplayName = "ShotgunEquipped")
};
