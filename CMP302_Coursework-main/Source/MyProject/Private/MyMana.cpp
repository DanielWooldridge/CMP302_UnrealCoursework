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

// Sets the Players Mana 
void UMyMana::setMana(float newMana)
{
	// If the Mana is set to more than the max mana return the max mana
	// This is used when the regenerated Mana outgrows the max mana - it will be set back to the max
	if ((currentMana = newMana) > maxMana)
	{
		currentMana = maxMana;
	}
	else
	{
		currentMana = newMana; // When using an ability that costs mana - It will set the current mana to the new mana value
	}
}

// Gets the Players Mana
float UMyMana::getMana()
{
	return currentMana;
}

// Sets the max Mana for that character
// Do this so that if new characters were added you can use this class and change the mana data
void UMyMana::setMaxMana(float newMaxMana)
{

	maxMana = newMaxMana;

}



