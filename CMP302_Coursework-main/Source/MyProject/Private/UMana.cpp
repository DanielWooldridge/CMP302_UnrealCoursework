// Fill out your copyright notice in the Description page of Project Settings.


#include "UMana.h"

// Sets default values
AUMana::AUMana()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	maxMana = 100.0f;
	currentMana = maxMana;

}

// Called when the game starts or when spawned
void AUMana::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUMana::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUMana::modifyMana(float deltaMana)
{
	currentMana = FMath::Clamp(currentMana + deltaMana, 0.0f, maxMana);
}

float AUMana::getCurrentMana() 
{
	return currentMana;
}
