#pragma once

UENUM(BlueprintType)
enum class ECharacterGuardState : uint8
{
	ECGS_Guarding UMETA(DisplayName = "Guarding"), //블루프린트에서 사용할 이름
	ECGS_Parrying UMETA(DisplayName = "Parrying"),
	ECGS_UnGuarded UMETA(DisplayName = "UnGuarded"),
	ECGS_GuardBroken UMETA(DisplayName = "GuardBroken")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_ParrySuccess UMETA(DisplayName = "ParrySuccess"),
	EAS_Hazard UMETA(DisplayName = "Hazard")
};
UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	EPS_Patrol UMETA(DisplayName = "Patrol"),
	EPS_BossFight UMETA(DisplayName = "BossFight")
};