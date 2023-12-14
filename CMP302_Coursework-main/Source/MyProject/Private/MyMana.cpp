// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMana.h"

// Sets default values for this component's properties
UMyMana::UMyMana()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMyMana::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMyMana::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UMyMana::setMana(float newMana)
{
	if ((currentMana = newMana) > maxMana)
	{
		currentMana = maxMana;
	}
	else
	{
		currentMana = newMana;
	}
}

float UMyMana::getMana()
{
	return currentMana;
}

void UMyMana::setMaxMana(float newMaxMana)
{

	maxMana = newMaxMana;

}



