// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

 
// Notes:
	// Mana is currently breaking my whole thing
	// Get this done before you do anything tomorrow

AMyProjectCharacter::AMyProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a Mana Component
	characterManaData = CreateDefaultSubobject<UMyMana>(TEXT("ManaData"));


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Setup Cooldown Variables
	canShoot = true;
	shootCooldown = 3.0f;

	canUlt = true;
	ultimateCooldown = 3.0f;

	// Setup the distance in which the projectiles will stop and spin around the player
	StandstillDistance = 500.0f;

	// Mana Object Decleration
	shootMana = 20;							// Shoot mana Cost
	ultMana = 35;							// Ult mana cost
	characterManaData->setMaxMana(100);		// Setting the Players Max Mana
	characterManaData->setMana(100);		// Setting the Players Starting Mana

	// Setup Display for Mana
	displayMana = 1.0f;						
	showManaRegen = true;
}

void AMyProjectCharacter::Tick(float DeltaTime)
{

	UI(DeltaTime);

	// This simulates a simple Cooldown System for the Players Abilities
	// If the canShoot == false
	if (!canShoot)
	{
		// Set the Shoot cooldown float variable to += DeltaTime 
		shootCooldown += DeltaTime;

		// When the Shoot Cooldown is greater or equal to 3
		if (shootCooldown >= 3.0f)
		{
			// Ensure the cooldown doesn't exceed the maximum value
			shootCooldown = 3.0f;
			// Set the canShoot value too true - This allows the player to then use their regular shoot ability
			canShoot = true;
		}
	}


	if (!canUlt)
	{
		ultimateCooldown += DeltaTime;
		if (ultimateCooldown >= 3.0f)
		{
			ultimateCooldown = 3.0f;
			canUlt = true;
		}
	}

	manaRegenTimer += DeltaTime;

	// This simulates a basic Mana Regeneration Feature
	// When the ManaRegenTimer is greater or equal to 1
	if (manaRegenTimer >= 1.0f) {

		// When the Characters Mana is less than 100
		if (characterManaData->getMana() < 100)
		{	
			// Add 5 to your characters Mana 
			characterManaData->setMana(characterManaData->getMana() + 5);
		}
		manaRegenTimer = 0.0f;
	}

	

	
}

void AMyProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}



}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Move);
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Look);
		//Shooting
		EnhancedInputComponent->BindAction(shootAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Shoot);		// Calls the Players Regular Ability
		//Ultimate
		EnhancedInputComponent->BindAction(ultimateAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Ultimate); // Calls the Players Ultimate Ability
	
	}

}

void AMyProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMyProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

// This is the Players Standard Ability
void AMyProjectCharacter::Shoot(const FInputActionValue& Value)
{
	// If the Player can Shoot - Cooldown Feature
	if (canShoot)
	{
		// And if the Player has enough Mana to Cast the Regular Ability
		if (characterManaData->getMana() > shootMana)
		{
			// Take the Shoot Mana Cost away from the Players current Mana
			characterManaData->setMana(characterManaData->getMana() - shootMana);

			//AbilityUsed->ShootShurikens();
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Q Ability Cast"));

			FVector PlayerLocation = GetActorLocation();					// Get the Players Current Location
			FVector PlayerForwardVector = GetActorForwardVector();			// Get the Players Current Forward Vector (The direction they are looking)
			
			// Middle Shuriken Values
			FVector LocationMiddle = PlayerLocation + PlayerForwardVector * 100.0f;	 // Shoot the Middle Projectile that will Travel 100 units in front of the player
			FRotator RotationMiddle = PlayerForwardVector.Rotation();				 // Set the Middle Projectiles Rotation to equal the Players Forward Vector

			FVector LocationLeft = PlayerLocation + PlayerForwardVector.RotateAngleAxis(-30.0f, FVector::UpVector) * 50.0f;		// Shoot the Left Projectile that will travel 50 units away from the Player and 30 degrees rotated from the Players Forward Vector 
			FRotator RotationLeft = PlayerForwardVector.RotateAngleAxis(-30.0f, FVector::UpVector).Rotation();					// Set the Left Projectiles Rotation to equal the Players Forward Vector rotated -30 degrees around the up vector

			FVector LocationRight = PlayerLocation + PlayerForwardVector.RotateAngleAxis(220.0f, FVector::UpVector) * 50.0f;	
			FRotator RotationRight = PlayerForwardVector.RotateAngleAxis(220.0f, FVector::UpVector).Rotation();					
				
			// Spawn middle projectile
			middleShuriken = GetWorld()->SpawnActor<AProjectile>(LocationMiddle, RotationMiddle);					// Spawn in the Middle Projectile with the location, rotation and distance it will travel from the Player
			middleShuriken->getAngleRotation();																		// Set the Middle Projectile to Spin using Angular Velocity
			middleShuriken->StaticMesh->SetPhysicsLinearVelocity(PlayerForwardVector * 2000.0f);					// Now Set the Moddle Projectile to have a linear Velocity of 2000 so that it moves in the location and orientation we have set it too

			// Spawn left projectile
			leftShuriken = GetWorld()->SpawnActor<AProjectile>(LocationLeft, RotationLeft);							
			leftShuriken->getAngleRotation();																		
			leftShuriken->StaticMesh->SetPhysicsLinearVelocity(leftShuriken->GetActorRightVector() * 2000.0f);		

			// Spawn right projectile
			rightShuriken = GetWorld()->SpawnActor<AProjectile>(LocationRight, RotationRight);						
			rightShuriken->getAngleRotation();																		
			rightShuriken->StaticMesh->SetPhysicsLinearVelocity(rightShuriken->GetActorRightVector() * 2000.0f);	



			// Adding shurikens to the array
			SpawnedProjectiles.Add(middleShuriken);
			SpawnedProjectiles.Add(leftShuriken);
			SpawnedProjectiles.Add(rightShuriken);

			// Setting a timer to return the projectiles at desired time - .2 seconds
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyProjectCharacter::ReturnProjectiles, 0.2f, false);

			// Reset Cooldown Varaiables so that the Player has to wait to use this ability again
			shootCooldown = 0.f;
			canShoot = false;
		}
		else {
			// If player does not have enough Mana - Display this
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Not Enough Mana!")));
		}
	}
	
}


// Return the Projectiles to the Player while being active
void AMyProjectCharacter::ReturnProjectiles()
{

	// Set a Vector to hold the Players location
	FVector PlayerLocation = GetActorLocation();

	// For each Projectile in the Array
	for (AProjectile* Projectile : SpawnedProjectiles)
	{
		// If the Projectile in the Array is true
		if (Projectile)
		{
			// Get the Static Mesh for the Projectile
			UStaticMeshComponent* ProjectileMesh = Projectile->StaticMesh;

			// If it has a StaticMesh
			if (ProjectileMesh)
			{
				// Create a Vector that holds the direction value from the projectile to the player
				FVector ReturnDirection = PlayerLocation - Projectile->GetActorLocation();
				ReturnDirection.Normalize();

				// Set the linear velocity to move the projectile towards the player
				float ReturnSpeed = 2000.0f; 
				ProjectileMesh->SetPhysicsLinearVelocity(ReturnDirection * ReturnSpeed);
			}
		}
	}

	// Timer for calling the delete fucntion
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyProjectCharacter::DeleteProjectiles, 0.2f, false);

	
}


void AMyProjectCharacter::DeleteProjectiles()
{
	// For each Projecitle in the array
	for (AProjectile* Projectile : SpawnedProjectiles)
	{
		if (Projectile)
		{
			// Destroy 
			Projectile->Destroy();
		}
	}
	// Empty the array
	SpawnedProjectiles.Empty(); 
}


void AMyProjectCharacter::Ultimate(const FInputActionValue& Value)
{

	// If the Player can use ultimate - Cooldown Feature
	if (canUlt)
	{
		// And if the Player has enough Mana to Cast the Ultimate Ability
		if (characterManaData->getMana() > ultMana)
		{
			// Take the Shoot Mana Cost away from the Players current Mana
			characterManaData->setMana(characterManaData->getMana() - ultMana);

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Ultimate here!")));

			// HIde Player when Active
			SetActorHiddenInGame(true);

			// Set the Number of shurikens that will be spawned
			const int32 NumberOfShurikens = 12;
			const float AngleDistanceInc = 360.0f / NumberOfShurikens;

			// Get the Players location
			FVector PlayerLocation = GetActorLocation();

			// Create an Array exclusive to the Ultimate
			TArray<AProjectile*> UltimateProjectiles;

			// Run this as for as many shurikens you set
			for (int32 i = 0; i < NumberOfShurikens; ++i)
			{
				// Calculate spawn location and rotation for the shuriken
				float RadianAngle = FMath::DegreesToRadians(i * AngleDistanceInc);	// Change angles from Degrees to Radians
				FVector ShurikenLocation = PlayerLocation + FVector(FMath::Cos(RadianAngle), FMath::Sin(RadianAngle), 0.0f) * 100.0f; // Calculate the spawn location based on polar coordinates
				FRotator ShurikenRotation = FRotationMatrix::MakeFromX(PlayerLocation - ShurikenLocation).Rotator(); // Calculate the spawn rotation

				// Spawn the shuriken with the correct location and orientation around the player
				AProjectile* UltimateShuriken = GetWorld()->SpawnActor<AProjectile>(ShurikenLocation, ShurikenRotation);
				UltimateShuriken->getAngleRotation();	// Add angular velocity
				UltimateShuriken->StaticMesh->SetPhysicsLinearVelocity(UltimateShuriken->GetActorForwardVector() * 2000.0f); // Add Linear Velocity at coded speed

				// Set the standstill distance for the spawned shuriken
				UltimateShuriken->StandstillDistance = StandstillDistance;

				// Set it so we know if it is an ultimate projectiles
				UltimateShuriken->bIsUltimateProjectile = true;

				// Add the ultimate projectile to the array we made
				UltimateProjectiles.Add(UltimateShuriken);

			}

			// Add the ultimate projectiles array to the main SpawnedProjectiles array
			SpawnedUltProjectiles.Append(UltimateProjectiles);

			// Reset the shooting ultimate cooldowns
			ultimateCooldown = 0.f;
			canUlt = false;

			// Timer for calling the delete function 
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyProjectCharacter::ReturnUltProjectiles, 2.f, false);
		}

		else {
			// If player does not have enough Mana - Display this
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Not Enough Mana!")));
		}
	}
}


void AMyProjectCharacter::ReturnUltProjectiles()
{
	FVector PlayerLocation = GetActorLocation();


	for (AProjectile* Projectile : SpawnedUltProjectiles)
	{
		if (Projectile)
		{
			// Assuming you have a reference to the StaticMesh component in your Projectile class
			UStaticMeshComponent* ProjectileMesh = Projectile->StaticMesh;

			if (ProjectileMesh)
			{

				// Calculate the direction from the projectile to the player
				FVector ReturnDirection = PlayerLocation - Projectile->GetActorLocation();
				ReturnDirection.Normalize();

				// Set the linear velocity to move the projectile towards the player
				float ReturnSpeed = 2000.0f; 
				ProjectileMesh->SetPhysicsLinearVelocity(ReturnDirection * ReturnSpeed);
			}
		}
	}

	SetActorHiddenInGame(false);

	// Set a timer to destroy the projectile mesh components after a delay
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyProjectCharacter::DeleteUltProjectiles, 0.2f, false);
	
}


void AMyProjectCharacter::DeleteUltProjectiles()
{
	// For each Projecitle in the array
	for (AProjectile* Projectile : SpawnedUltProjectiles)
	{
		if (Projectile)
		{
			// Destroy
			Projectile->Destroy();
		}
	}
	// Empty the array
	SpawnedUltProjectiles.Empty(); 
	
}


void AMyProjectCharacter::UI(float DeltaTime)
{

	// Create 2 Static bools to use for the On Screen UI Display - 1 for each ability
	static bool talonQReady = true;
	static bool talonUltReady = true;

	// Display a message indicating that you can shoot if talonQReady is true and if canShoot is true
	if (canShoot && talonQReady)
	{
		// Print message saying its ready to be used
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Q Ability Available")));
		// Set Boolean to false
		talonQReady = false;
	}
	else if(!canShoot)
	{
		// Remove the message when you can't shoot
		GEngine->ClearOnScreenDebugMessages();
		// Set Boolean to true
		talonQReady = true;
		
	}

	// Display a message indicating that you can use your ultimate
	if (canUlt && talonUltReady)
	{
		// Display message saying its ready
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Ultimate Ability Available")));
		// Set Boolean to false
		talonUltReady = false;
	}
	else if (!canUlt)
	{
		// Remove the message when you can't use your ultimate
		GEngine->ClearOnScreenDebugMessages();
		// Set Boolean to true
		talonUltReady = true;
	}

	// Mana Text Display
	// If the display mana variable is above 1
	if (displayMana += DeltaTime >= 1)
	{
		// Set Show Mana Regen to true
		showManaRegen = true;
	}
	// If the Show Mana Regen Boolean is true
	if (showManaRegen)
	{	
		// Display what the players Current Mana is 
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, FString::Printf(TEXT("Mana: %.2f"), characterManaData->getMana()));
		// Set the Show Mana Regen Boolean to false
		showManaRegen = false;
	}
	
}



