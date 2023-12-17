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

	canShoot = true;
	shootCooldown = 3.0f;

	canUlt = true;
	ultimateCooldown = 3.0f;

	StandstillDistance = 500.0f;

	//returnStaticPlayer = true;

	// Mana Object Decleration
	shootMana = 20;
	ultMana = 35;
	characterManaData->setMaxMana(100);
	characterManaData->setMana(100);
	displayMana = 1.0f;
	showManaRegen = true;
}

void AMyProjectCharacter::Tick(float DeltaTime)
{

	UI(DeltaTime);


	if (!canShoot)
	{
		shootCooldown += DeltaTime;

		if (shootCooldown >= 3.0f)
		{
			shootCooldown = 3.0f; // Ensure the cooldown doesn't exceed the maximum value
			canShoot = true;
		}
	}

	if (!canUlt)
	{
		ultimateCooldown += DeltaTime;

		if (ultimateCooldown >= 3.0f)
		{
			ultimateCooldown = 3.0f; // Ensure the cooldown doesn't exceed the maximum value
			canUlt = true;
		}
	}

	manaRegenTimer += DeltaTime;

	if (manaRegenTimer >= 1.0f) {

		if (characterManaData->getMana() < 100)
		{	
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
		EnhancedInputComponent->BindAction(shootAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Shoot);
		//Ultimate
		EnhancedInputComponent->BindAction(ultimateAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Ultimate);
	
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

void AMyProjectCharacter::Shoot(const FInputActionValue& Value)
{
	//TalonQ->ShootQ(Value);
	
	if (canShoot)
	{
		if (characterManaData->getMana() > shootMana)
		{
			characterManaData->setMana(characterManaData->getMana() - shootMana);

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Shoot Successfull!"));

			/////// - READ -  doesn't apply damage - GetRotation orks for right and left shuriken but not midddle one, its also not showing message, when this is implemented this is done////

			// Calculate positions and rotations for middle, left, and right projectiles
			FVector PlayerLocation = GetActorLocation();
			FVector PlayerForwardVector = GetActorForwardVector();

			FVector LocationMiddle = PlayerLocation + PlayerForwardVector * 100.0f; // Adjust distance from the player
			FRotator RotationMiddle = PlayerForwardVector.Rotation();

			FVector LocationLeft = PlayerLocation + PlayerForwardVector.RotateAngleAxis(-30.0f, FVector::UpVector) * 50.0f; // Adjust angle and distance
			FRotator RotationLeft = PlayerForwardVector.RotateAngleAxis(-30.0f, FVector::UpVector).Rotation();

			FVector LocationRight = PlayerLocation + PlayerForwardVector.RotateAngleAxis(220.0f, FVector::UpVector) * 50.0f; // Adjust angle and distance
			FRotator RotationRight = PlayerForwardVector.RotateAngleAxis(220.0f, FVector::UpVector).Rotation();

			// Spawn middle projectile
			middleShuriken = GetWorld()->SpawnActor<AProjectile>(LocationMiddle, RotationMiddle);
			middleShuriken->getAngleRotation();
			middleShuriken->StaticMesh->SetPhysicsLinearVelocity(PlayerForwardVector * 2000.0f); // Adjust speed as needed

			// Spawn left projectile
			leftShuriken = GetWorld()->SpawnActor<AProjectile>(LocationLeft, RotationLeft);
			leftShuriken->getAngleRotation();
			leftShuriken->StaticMesh->SetPhysicsLinearVelocity(leftShuriken->GetActorRightVector() * 2000.0f); // Adjust speed as needed

			// Spawn right projectile
			rightShuriken = GetWorld()->SpawnActor<AProjectile>(LocationRight, RotationRight);
			rightShuriken->getAngleRotation();
			rightShuriken->StaticMesh->SetPhysicsLinearVelocity(rightShuriken->GetActorRightVector() * 2000.0f); // Adjust speed as needed


			// Adding shurikens to the array
			SpawnedProjectiles.Add(middleShuriken);
			SpawnedProjectiles.Add(leftShuriken);
			SpawnedProjectiles.Add(rightShuriken);

			// Setting a timer to return the projectiles at desired time - .2 seconds
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyProjectCharacter::ReturnProjectiles, 0.2f, false);

			shootCooldown = 0.f;
			canShoot = false;
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Not Enough Mana!")));
		}
	}
	
}


void AMyProjectCharacter::ReturnProjectiles()
{
	FVector PlayerLocation = GetActorLocation();


	for (AProjectile* Projectile : SpawnedProjectiles)
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
				float ReturnSpeed = 2000.0f; // Adjust the speed as needed
				ProjectileMesh->SetPhysicsLinearVelocity(ReturnDirection * ReturnSpeed);
			}
		}
	}

	// Set a timer to destroy the projectile mesh components after a delay
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyProjectCharacter::DeleteProjectiles, 0.2f, false);

	
}


void AMyProjectCharacter::DeleteProjectiles()
{
	// Destroy the projectiles
	for (AProjectile* Projectile : SpawnedProjectiles)
	{
		if (Projectile)
		{
			Projectile->Destroy();
		}
	}
	SpawnedProjectiles.Empty(); // Clear the array
}


void AMyProjectCharacter::Ultimate(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Ultimate here!")));

	if (canUlt)
	{
		
		if (characterManaData->getMana() > ultMana)
		{
			characterManaData->setMana(characterManaData->getMana() - ultMana);

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);

			const int32 NumShurikens = 12;
			const float AngleIncrement = 360.0f / NumShurikens;

			FVector PlayerLocation = GetActorLocation();

			// Create a separate array for ultimate projectiles
			TArray<AProjectile*> UltimateProjectiles;

			for (int32 i = 0; i < NumShurikens; ++i)
			{
				// Calculate spawn location and rotation for each shuriken
				float AngleInRadians = FMath::DegreesToRadians(i * AngleIncrement);
				FVector SpawnLocation = PlayerLocation + FVector(FMath::Cos(AngleInRadians), FMath::Sin(AngleInRadians), 0.0f) * 100.0f;
				FRotator SpawnRotation = FRotationMatrix::MakeFromX(PlayerLocation - SpawnLocation).Rotator();

				// Spawn the shuriken
				AProjectile* UltimateShuriken = GetWorld()->SpawnActor<AProjectile>(SpawnLocation, SpawnRotation);
				UltimateShuriken->getAngleRotation();
				UltimateShuriken->StaticMesh->SetPhysicsLinearVelocity(UltimateShuriken->GetActorForwardVector() * 2000.0f); // Adjust speed as needed

				// Set the standstill distance for the spawned shuriken
				UltimateShuriken->StandstillDistance = StandstillDistance;

				// Set the flag for ultimate projectiles
				UltimateShuriken->bIsUltimateProjectile = true;

				// Add the ultimate projectile to the separate array
				UltimateProjectiles.Add(UltimateShuriken);

			}

			// Add the ultimate projectiles array to the main SpawnedProjectiles array
			SpawnedUltProjectiles.Append(UltimateProjectiles);
			SetActorEnableCollision(true);


			ultimateCooldown = 0.f;
			canUlt = false;

			// Set a timer to return the ultimate projectiles at the desired time - 2 seconds
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyProjectCharacter::ReturnUltProjectiles, 2.f, false);
		}

		else {
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
				float ReturnSpeed = 2000.0f; // Adjust the speed as needed
				ProjectileMesh->SetPhysicsLinearVelocity(ReturnDirection * ReturnSpeed);
			}

			// For deleting Projectiles one by one
			/*if (Projectile->GetActorLocation() + 10 == GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("HITTING!")));
				DeleteUltProjectiles();
			}*/

			
		}
	}

	SetActorHiddenInGame(false);

	// Set a timer to destroy the projectile mesh components after a delay
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyProjectCharacter::DeleteUltProjectiles, 0.2f, false);
	
}


void AMyProjectCharacter::DeleteUltProjectiles()
{
	// Destroy the projectiles
	for (AProjectile* Projectile : SpawnedUltProjectiles)
	{
		if (Projectile)
		{
			Projectile->Destroy();
		}
	}
	SpawnedUltProjectiles.Empty(); // Clear the array
	//returnStaticPlayer = true; // Sets this too true so you can recieve another static player positiuon when you press the ultimate button so shurikens dont ravel away from the new location
}


void AMyProjectCharacter::UI(float DeltaTime)
{

	static bool talonQReady = true;
	static bool talonUltReady = true;

	// Display a message indicating that you can shoot
	if (canShoot && talonQReady)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Q Ability Available")));
		talonQReady = false;
	}
	else if(!canShoot)
	{
		// Remove the message when you can't shoot
		talonQReady = true;
		GEngine->ClearOnScreenDebugMessages();
		
	}

	// Display a message indicating that you can use your ultimate
	if (canUlt && talonUltReady)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Ultimate Ability Available")));
		talonUltReady = false;
	}
	else if (!canUlt)
	{
		// Remove the message when you can't use your ultimate
		talonUltReady = true;
		GEngine->ClearOnScreenDebugMessages();
	}

	// Mana Text - randomly explodes tons of messages 
	if (displayMana += DeltaTime >= 1)
	{
		showManaRegen = true;
	}
	if (showManaRegen)
	{	
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, FString::Printf(TEXT("Mana: %.2f"), characterManaData->getMana()));
		showManaRegen = false;
	}
	
}

FVector AMyProjectCharacter::getPlayerPosition()
{
	return GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
}

