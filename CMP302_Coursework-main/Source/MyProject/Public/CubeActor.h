// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "CubeActor.generated.h"

UCLASS()
class MYPROJECT_API ACubeActor : public AActor
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	ACubeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Variables
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticCubeMesh;




};
