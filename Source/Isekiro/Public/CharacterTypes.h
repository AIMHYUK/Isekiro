#pragma once

UENUM(BlueprintType)
enum class ECharacterGuardState : uint8
{
	ECGS_Guarding UMETA(DisplayName = "Guarding"), //블루프린트에서 사용할 이름
	ECGS_Parrying UMETA(DisplayName = "Parrying"),
	ECGS_UnGuarded UMETA(DisplayName = "UnGuarded")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking")
};