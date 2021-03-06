// Copyright© SD5 - Sean Dewar, 2015.

#pragma once

#include "GameFramework/Character.h"
#include "SD5BunnyGunDamageType.h"
#include "SD5BunnyGunCharacter.generated.h"

/**
* Replicated info about a hit that a Bunny Gun character has taken.
*/
USTRUCT()
struct FHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float ActualDamage;

	UPROPERTY()
	uint32 bIsKillingHit : 1; 
	
	UPROPERTY()
	TSubclassOf<UDamageType> DamageTypeClass;
	
	UPROPERTY()
	TWeakObjectPtr<APawn> InstigatorPawn;

	UPROPERTY()
	TWeakObjectPtr<AActor> DamageCauser;

	FHitInfo();
};

// The time period in which a new LastHitInfo is not replicated anymore.
// Stops hit FX to be spammed on clients joining mid-game.
#define LAST_HIT_INFO_TIMEOUT_SECONDS 0.5f

/**
* The character actor for the Bunny Gun character.
*/
UCLASS()
class SD5BUNNYGUN_API ASD5BunnyGunCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASD5BunnyGunCharacter(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	void BeginPlay() override;
	
	// Called every frame
	void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Called on actor before replication occurs.
	void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

	// Updates the camera according to the player controller's rotation.
	void UpdateCamera();

	// Updates the networked view look rotation of the player.
	void UpdateLookRotation();

	// Damages the player and lets the game know what/who is responsible for applying the damage.
	float TakeDamage(float Damage, const FDamageEvent& DamageEvent = FDamageEvent(), AController* EventInstigator = nullptr, AActor* DamageCauser = nullptr) override;

	// Replicates a hit to other clients and calls OnHit for the authority.
	void HandleHit(float ActualDamage, bool bIsKillingHit, const TSubclassOf<UDamageType>& DamageTypeClass, APawn* InstigatorPawn, AActor* DamageCauser);

	// Called when the player is hit.
	void OnHit(float Damage, bool bIsKillingHit, const TSubclassOf<UDamageType>& DamageTypeClass, APawn* InstigatorPawn, AActor* DamageCauser);

	// Handle hits on the client and deaths.
	UFUNCTION()
	void OnRep_LastHitInfo();

	// Handles the death of the player. Returns true if successful, false if not.
	bool HandleDeath(float ActualDamage, const TSubclassOf<UDamageType>& DamageTypeClass, AController* Killer, AActor* DamageCauser);

	// Called when the player is killed.
	void OnDeath(float KillingDamage, const TSubclassOf<UDamageType>& DamageTypeClass, APawn* KillerPawn, AActor* DamageCauser);

	// Ragdolls the character. Returns true if the character successfully ragdolled, false if we couldn't (character will be hidden instead).
	bool RagdollCharacter();

	// Kills the player.
	void Kill(const FDamageEvent& DamageEvent = FDamageEvent(), AController* Killer = nullptr, AActor* DamageCauser = nullptr);

	// Kills the player via suicide and lets the game know the player killed themselves.
	void Suicide();

	// Suicides the player on the server.
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSuicide();

	// Returns whether or not the character's health is above 0.
	bool IsAlive() const;

	// Called when the player jumps.
	void OnJumped_Implementation() override;

	// Plays the jump sound.
	void PlayJumpSound();

	// Called when the player lands.
	void Landed(const FHitResult& Hit) override;

	// Handles fall damage and FX if enabled.
	void HandleFallDamage();

	// Updates the fall damage camera tilt.
	void UpdateFallDamageCameraTilt(float DeltaTime);

	// Clears the current jump input unless the player is using autohop while holding down jump.
	void ClearJumpInput() override;

	// Tries to start crouching.
	void StartCrouching();

	// Tries to stop crouching.
	void StopCrouching();

	// Makes the player slowly walk.
	void StartSlowWalking();

	// Stops the player from slowly walking if they were doing so.
	void StopSlowWalking();

	// Turning for controller input (or for other input devices that use a rate)
	void TurnAtRate(float Rate);

	// Looking for controller input (or for other input devices that use a rate)
	void LookUpAtRate(float Rate);

	// Moves the character forward or backwards.
	void MoveForward(float Val);

	// Moves the character right or left.
	void MoveRight(float Val);

	// Camera used when in first person mode.
	UPROPERTY(Category = "Character: Camera", VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* FirstPersonCamera;

	// Text that appears above the player's head.
	UPROPERTY(Category = "Character", VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* PlayerText;

	// The current health of the character.
	UPROPERTY(Category = "Character: Health", Replicated, EditAnywhere, BlueprintReadWrite)
	float Health;

	// Whether or not the character is currently in the process of dying.
	UPROPERTY(Category = "Character: Health", Transient, VisibleAnywhere, BlueprintReadOnly)
	uint32 bIsDying : 1;

	// Information about the last hit that this character has taken.
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastHitInfo)
	FHitInfo LastHitInfo;

	// The timestamp in which LastHitInfo is not replicated anymore.
	float LastHitInfoTimeoutStamp;

	// Whether or not the jump key can be held in order to automatically bunnyhop for this character.
	UPROPERTY(Category = "Character: Bunnyhopping", Replicated, EditAnywhere, BlueprintReadWrite)
	uint32 bUseAutoHop : 1;

	// Whether or not the character can take fall damage.
	UPROPERTY(Category = "Character: Fall Damage", Replicated, EditAnywhere, BlueprintReadWrite)
	uint32 bEnableFallDamage : 1;

	// Minimum amount of negative velocity needed in the Z direction in order to take fall damage.
	UPROPERTY(Category = "Character: Fall Damage", Replicated, EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnableFallDamage", ClampMin = "0", UIMin = "0"))
	float FallDamageMinZVelocity;

	// After reaching FallDamageZVelocity, the difference between the player's downward velocity and FallDamageZVelocity is multiplied by this value
	// and is given to the player as damage.
	UPROPERTY(Category = "Character: Fall Damage", Replicated, EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnableFallDamage", ClampMin = "0", UIMin = "0"))
	float FallDamageZVelocityMultiplier;

	// Whether or not to use the character's current horizontal acceleration (of the movement component) on deciding which direction to tilt the camera on fall damage impact.
	// If false, uses character's horizontal velocity instead.
	UPROPERTY(Category = "Character: Fall Damage", Replicated, EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnableFallDamage"))
	uint32 bUseAccelerationForFallDamageCameraTilt : 1;

	// The amount of camera tilt from taking fall damage is scaled by this value. (Setting this to 0 disables the camera tilt effect).
	UPROPERTY(Category = "Character: Fall Damage", Replicated, EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnableFallDamage", ClampMin = "0", UIMin = "0"))
	float FallDamageCameraTiltMultiplier;

	// The maximum amount of camera tilt.
	UPROPERTY(Category = "Character: Fall Damage", Replicated, EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnableFallDamage", ClampMin = "0", UIMin = "0"))
	float MaxFallDamageCameraTilt;

	// The amount of decay in the camera tilt (higher value decreases the camera tilt time)
	UPROPERTY(Category = "Character: Fall Damage", Replicated, EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnableFallDamage", ClampMin = "0", UIMin = "0"))
	float FallDamageCameraTiltDecayMultiplier;

	// The current camera tilt value from fall damage.
	float FallDamageCameraTilt;

	// Base turn rate, in deg/sec. Other scaling may affect final turn rate.
	UPROPERTY(Category = "Character: Controls", VisibleAnywhere, BlueprintReadOnly)
	float BaseTurnRate;

	// Base look up/down rate, in deg/sec. Other scaling may affect final rate.
	UPROPERTY(Category = "Character: Controls", VisibleAnywhere, BlueprintReadOnly)
	float BaseLookUpRate;

	// The current networked view look direction of the player controller.
	UPROPERTY(Category = "Character: Look", Transient, Replicated, VisibleAnywhere, BlueprintReadOnly)
	FRotator LookRotation;

	// This audio component is responsible for playing the jump sound correctly (doesn't spam multiple jump sounds).
	UPROPERTY()
	UAudioComponent* JumpAudio;

	// The sound played when the character performs a successful jump.
	UPROPERTY(Category = "Character: Sounds", EditDefaultsOnly)
	USoundCue* JumpSound;

	// The sound played when the character takes fall damage.
	UPROPERTY(Category = "Character: Sounds", EditDefaultsOnly)
	USoundCue* FallDamageSound;

	// The sound played when the character takes non-lethal damage.
	UPROPERTY(Category = "Character: Sounds", EditDefaultsOnly)
	USoundCue* HitSound;

	// The sound played when the character dies.
	UPROPERTY(Category = "Character: Sounds", EditDefaultsOnly)
	USoundCue* DeathSound;
};
