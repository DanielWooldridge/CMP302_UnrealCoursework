// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Projectile.generated.h"

UCLASS()
class MYPROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void getAngleRotation();

protected:


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Variables
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float StandstillDistance;

	// Flag indicating whether it's an ultimate projectile
	bool bIsUltimateProjectile;
	

};
