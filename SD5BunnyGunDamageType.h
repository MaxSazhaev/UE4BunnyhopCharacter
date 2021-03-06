// Copyright© SD5 - Sean Dewar, 2015.

#pragma once

#include "GameFramework/DamageType.h"
#include "SD5BunnyGunDamageType.generated.h"

/**
 * Standard generic damage type for Bunny Gun.
 */
UCLASS()
class SD5BUNNYGUN_API USD5BunnyGunDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	USD5BunnyGunDamageType();

	// Whether or not the hit sound should be played when the player has been killed by this damage type.
	UPROPERTY(Category = "Sounds", EditDefaultsOnly)
	uint32 bPlayHitSoundWhenKilled : 1;

	// The hit sound to be played when damaged by this damage type.
	UPROPERTY(Category = "Sounds", EditDefaultsOnly)
	USoundCue* HitSound;

	// The death sound to be played when killed by this damage type.
	UPROPERTY(Category = "Sounds", EditDefaultsOnly)
	USoundCue* DeathSound;
};
