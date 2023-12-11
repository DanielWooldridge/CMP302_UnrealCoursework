// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InputActionValue.h"

//Time Header File
#include "Engine/TimerHandle.h"

//Projectile Header Files
#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "TalonQ.generated.h"

UCLASS()
class MYPROJECT_API ATalonQ : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATalonQ();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Called for shooting input */
	void ShootQ(const FInputActionValue& Value);

	/* Function to return projectiles to the player and then deletes them */
	void ReturnProjectiles();
	void DeleteProjectiles();

	/* Projecile */
	TArray<AProjectile*> SpawnedProjectiles;

	/* Cooldown Timer */
	float shootCooldown;
	bool canShoot;

};
