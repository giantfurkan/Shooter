#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubMachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")

};