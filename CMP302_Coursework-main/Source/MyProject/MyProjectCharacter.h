// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

// Time Header File
#include "Engine/TimerHandle.h"

// Projectile Header Files
#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"


// Mana Class Header File
#include "MyMana.h"
#include "Kismet/GameplayStatics.h"

#include "MyProjectCharacter.generated.h"


UCLASS(config=Game)
class AMyProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/* Action Input - Regular Shooting Ability */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* shootAction;
	
	/* Action Input - Ultimate Ability */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ultimateAction;


	
public:
	AMyProjectCharacter();
	
	virtual void Tick(float DeltaTime) override;
	// To add mapping context
	virtual void BeginPlay();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/* Called for shooting input */
	void Shoot(const FInputActionValue& Value);

	/* Called for Ultimate input */
	void Ultimate(const FInputActionValue& Value);

	/* Function to return projectiles to the player and then deletes them - For Regular Shooting Ability */
	void ReturnProjectiles();
	void DeleteProjectiles();

	/* Function to return projectiles to the player and then deletes them - For Ultimate Ability */
	void ReturnUltProjectiles();
	void DeleteUltProjectiles();

	/* UI Function */
	void UI(float DeltaTime);

	/* Projectile Class Variables */
	TArray<AProjectile*> SpawnedProjectiles;	// Holds the spawned projectiles from the regular shooting ability
	TArray<AProjectile*> SpawnedUltProjectiles; // Holds the spawned projectiles from the Ultimate ability
	AProjectile* middleShuriken;				// Create variables to hold the data for the different shurikens in the regular Shoot ability
	AProjectile* rightShuriken;		
	AProjectile* leftShuriken;		
	float StandstillDistance;					// Distance of which the shurikens come to a standstill

	/* Cooldown Timers */
	float shootCooldown;	// Cooldowns for the Regular shoot ability
	bool canShoot;
	float ultimateCooldown;	// Cooldowns for the Ultimate ability
	bool canUlt;
	

	/* Mana class Variables */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMyMana* characterManaData; // Holds the players mana

	float shootMana;		// The mana cost for using the Regular shoot ability
	float ultMana;			// The mana cost for using the Ultimate ability	
	float manaRegenTimer;	// The Timer which is used for the Mana regeneration cooldown
	float displayMana;		// Display and Timer for Mana
	bool showManaRegen;	

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

