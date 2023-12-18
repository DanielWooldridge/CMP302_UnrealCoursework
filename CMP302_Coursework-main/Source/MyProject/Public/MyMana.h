// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyMana.generated.h"

// the declare thingy 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyMana : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyMana();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Create functions that will handle the mana data
	void setMana(float);	
	float getMana();
	void setMaxMana(float);

	// Variables
	UPROPERTY(EditAnywhere)
	float maxMana;

	UPROPERTY(EditAnywhere)
	float currentMana;

		
};
