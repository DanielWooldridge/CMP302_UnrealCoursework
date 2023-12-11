// Fill out your copyright notice in the Description page of Project Settings.


#include "TalonQ.h"

// Sets default values
ATalonQ::ATalonQ()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	canShoot = true;
	shootCooldown = 3.f;
}

// Called when the game starts or when spawned
void ATalonQ::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATalonQ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!canShoot)
	{
		shootCooldown += DeltaTime;

		if (shootCooldown >= 3.0f)
		{
			shootCooldown = 3.0f; // Ensure the cooldown doesn't exceed the maximum value
			canShoot = true;
		}
	}

}

void ATalonQ::ShootQ(const FInputActionValue& Value)
{

	if (canShoot)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Shoot Successfull!"));

		/////// - READ - THE COOLDOWN FEATURE DOESNT WORK - DOESNT DELTE AFTER RETURNING TO PLAYER - doesnt apply damage////

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
		AProjectile* middleShuriken = GetWorld()->SpawnActor<AProjectile>(LocationMiddle, RotationMiddle);
		middleShuriken->StaticMesh->SetPhysicsLinearVelocity(PlayerForwardVector * 2000.0f); // Adjust speed as needed

		// Spawn left projectile
		AProjectile* leftShuriken = GetWorld()->SpawnActor<AProjectile>(LocationLeft, RotationLeft);
		leftShuriken->StaticMesh->SetPhysicsLinearVelocity(leftShuriken->GetActorRightVector() * 2000.0f); // Adjust speed as needed

		// Spawn right projectile
		AProjectile* rightShuriken = GetWorld()->SpawnActor<AProjectile>(LocationRight, RotationRight);
		rightShuriken->StaticMesh->SetPhysicsLinearVelocity(rightShuriken->GetActorRightVector() * 2000.0f); // Adjust speed as needed


		// Adding shurikens to the array
		SpawnedProjectiles.Add(middleShuriken);
		SpawnedProjectiles.Add(leftShuriken);
		SpawnedProjectiles.Add(rightShuriken);

		// Setting a timer to return the projectiles at desired time - 1 second
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ATalonQ::ReturnProjectiles, 0.2f, false);

		shootCooldown = 0.f;
		canShoot = false;
	}

}


void ATalonQ::ReturnProjectiles()
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
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ATalonQ::DeleteProjectiles, 0.2f, false);


}


void ATalonQ::DeleteProjectiles()
{
	// Destroy the projectiles
	for (AProjectile* Projectile : SpawnedProjectiles)
	{
		if (Projectile)
		{
			Projectile->MarkPendingKill();
		}
	}
	SpawnedProjectiles.Empty(); // Clear the array
}

