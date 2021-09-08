// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimeInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),
	EAT_MAX UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

		// scene component to use for its location for interping
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* SceneComponent;

	// number of items interping to/at this scene comp location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);


UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called for forwards/backward input 
	void MoveForward(float Value);

	// Called for side to side input
	void MoveRight(float Value);

	/*
	Called via input to turn at a given rate
	@param Rate This is a normalized rate, i.e 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/*
	Called via input to look up/down at a given rate.
	@param Rate This is a normalized rate, i.e 1.0 means 100% of desired rate
	*/
	void LookUpAtRate(float Rate);

	// Rotate controller based on mouse X movement
	// @param Value The Input value from mouse movement
	void Turn(float Value);

	// Rotate controller based on mouse Y movement
	// @param Value The Input value from mouse movement
	void LookUp(float Value);

	// called when the fire button is pressed
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	// Set bAiming to true or false
	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);

	// set baseturnrate and baselookuprate based on aiming
	void SetLookRates();

	void CalculateCrosshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();

	UFUNCTION()
		void FinishCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
		void AutoFireReset();

	// line trace for items under the crosshairs
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	// trace for items if overlapped item count > 0
	void TraceForItems();

	// spawns a default weapon and equips it
	class AWeapon* SpawnDefaultWeapon();

	// takes a weapon and attaches it to the mesh
	void EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping = false);

	// detach weapon and let it fall to the ground
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	// drops currently equipped weapon and equips tracehititem
	void SwapWeapon(AWeapon* WeaponToSwap);

	// initialize the ammo map with ammo values
	void InitializeAmmoMap();

	// check to make sure our weapon has ammo
	bool WeaponHasAmmo();

	// fireWeapon functions
	void PlayFireSound();
	void SendBullet();
	void PlayGunfireMontage();

	// bound to the R key and gamepad face button left
	void ReloadButtonPressed();

	// handle reloading of the weapon
	void ReloadWeapon();

	// checks to see if we have ammo of the equippedWeapon's data type
	bool CarryingAmmo();

	// called from animation blueprint with grab clip notify
	UFUNCTION(BlueprintCallable)
		void GrabClip();

	// called from animation blueprint with release clip notify
	UFUNCTION(BlueprintCallable)
		void ReleaseClip();

	void CrouchButtonPressed();

	virtual void Jump() override;

	// interps capsule half height when crouching/standing
	void InterpCapsuleHalfHeight(float DeltaTime);

	void Aim();
	void StopAiming();

	void PickupAmmo(class AAmmo* Ammo);

	void InitializeInterpLocation();

	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

	int32 GetEmptyInventorySlot();

	void HighlightInventorySlot();

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	// Camera that follows the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	// Base turn rate, in deg/sec. Other scaling may affect final turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float BaseTurnRate;

	// Base look up/down rate in deg/sec. Other scaling may effect final turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float BaseLookUpRate;

	// look up rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float HipTurnRate;

	// turn rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float HipLookUpRate;

	// Turn rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float AimingTurnRate;

	// look up rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float AimingLookUpRate;

	// scale factor for mouse look sensitivity. Turn rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MouseHipTurnRate;

	// scale factor for mouse look sensitivity. look up rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MouseHipLookUpRate;

	// scale factor for mouse look sensitivity. Turn rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MouseAimingTurnRate;

	// scale factor for mouse look sensitivity. look up rate when  aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MouseAimingLookUpRate;

	// montage for firing the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* HipFireMontage;

	// particles spawend upon bullet impact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* ImpactParticles;

	// smoke trail for bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* BeamParticles;

	// True when aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bAiming;

	// Default camera filed of view value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float CameraDefaultFOV;

	// Field of view value for when zoomed in
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float CameraZoomedFOV;

	// Current field of view this frame
	float CameraCurrentFOV;

	// Interp spped for zooming when aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float ZoomInterpSpeed;

	// determines the spread of the crosshairs
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
		float CrosshairSpreadMultiplier;

	// velocity component for crosshairs spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
		float CrosshairVelocityFactor;

	// in air component for crosshairs spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
		float CrosshairInAirFactor;

	// aim component for crosshairs spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
		float CrosshairAimFactor;

	// shooting component for crosshairs spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
		float CrosshairShootingFactor;

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	// left mouse button or right console trigger pressed
	bool bFireButtonPressed;

	// true when we can fire. false when waiting for the timer
	bool bShouldFire;

	// sets a timer between gunshoots
	FTimerHandle AutoFireTimer;

	// true if we should trace every frame for items
	bool bShouldTraceForItems;

	// number of overlapped AItems
	int8 OverlappedItemCount;

	// the AItem we hit last frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
		class AItem* TraceHitItemLastFrame;

	// currently equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
		class AWeapon* EquippedWeapon;

	// set this in blueprionts for the default weapon class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AWeapon> DefaultWeaponClass;

	// the item currently hit by out trace in TraceForItems (could be null)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		AItem* TraceHitItem;

	// distance outward from the camera for the interp destination
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
		float CameraInterpDistance;

	// distance upward from the camera for the interp destination
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
		float CameraInterpElevation;

	// map to keep track of ammo of the different ammo types
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
		TMap<EAmmoType, int32> AmmoMap;

	// Starting amount of 9mm ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
		int32 Starting9mmAmmo;

	// starting amount of AR ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
		int32 StartingARAmmo;

	// combat state can only fire or reload if unoccupied
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		ECombatState CombatState;

	// montage for reload animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* ReloadMontage;

	// montage for equip animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* EquipMontage;

	UFUNCTION(BlueprintCallable)
		void FinishReloading();

	UFUNCTION(BlueprintCallable)
		void FinishEquipping();

	// transform of the clip when we first grab the clip during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		FTransform ClipTransform;

	// scene component to attach to the character's hand during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		USceneComponent* HandSceneComponent;

	// true when crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bCrouching;

	// regular movement speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float BaseMovementSpeed;

	// crouch movement speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float CrouchMovementSpeed;

	// current half height of the capsule
	float CurrentCapsuleHalfHeight;

	// half height of the capsule when not crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float StandingCapsuleHalfHeight;

	// half height of the capsule when crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float CrouchingCapsuleHalfHeight;

	// ground friction while not crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float BaseGroundFriction;

	// ground friction while crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float CrouchingGroundFriction;

	// used for knowing when the aiming button is pressed
	bool bAimingButtonPressed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* WeaponInterpComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp6;

	// array of the interp location structs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TArray<FInterpLocation> InterpLocations;

	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	// time to wait before we can play another pickup sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
		float PickupSoundResetTime;

	// time to wait before we can play another equip sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
		float EquipSoundResetTime;

	// an array of AItems for our Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		TArray<AItem*> Inventory;

	const int32 INVENTORY_CAPACITY{ 6 };

	// Delegate for sending slot information to InventoryBar when equipping
	UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
		FEquipItemDelegate EquipItemDelegate;

	// delegate for sending slot information for playing the icon animation
	UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
		FHighlightIconDelegate HighlightIconDelegate;

	// the index for the currently highlighted slot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		int32 HighlightedSlot;

public:
	// Returns CameraBoom subobject
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; };

	// Returns FollowCamera subobject
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; };

	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
		float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	// add/substracts to/from overlappeditemcount and updates bShouldTraceForItems
	void IncrementOverlappedItemCount(int8 Amount);

	// no longer needed; AItem has GetInterpLocation
	//FVector GetCameraInterpLocation();

	void GetPickupItem(AItem* Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FInterpLocation GetInterpLocation(int32 index);

	// returns the index in InterpLocations array with the lowest itemCount
	int32 GetInterpLocationIndex();

	void IncrementInterpLocItemCount(int32 index, int32 Amount);

	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	void StartPickupSoundTimer();
	void StartEquipSoundTimer();

	void UnHighlightInventorySlot();

	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
};
