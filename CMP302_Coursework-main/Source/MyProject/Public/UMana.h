// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UMana.generated.h"

UCLASS()
class MYPROJECT_API AUMana : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUMana();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float getCurrentMana();
	void modifyMana(float deltaMana);

	UPROPERTY(VisibleAnywhere, Category = "Mana")
	float maxMana;

	UPROPERTY(VisibleAnywhere, Category = "Mana")
	float currentMana;

};
