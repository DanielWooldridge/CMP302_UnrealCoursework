#include "UMana.h"

// Sets default values
AUMana::AUMana()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	currentMana = 0.0f;
	maxMana = 0.0f;

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


void AUMana::setMana(float newMana)
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

float AUMana::getMana()
{
	return currentMana;
}

void AUMana::setMaxMana(float newMaxMana)
{

	maxMana = newMaxMana;
	
}
















// Debug Lines

//float AUMana::getMaxMana(float tempMaxMana)
//{
//	tempMaxMana = maxMana;
//	return tempMaxMana;
//}
//
//void AUMana::modifyMana(float deltaMana)
//{
//	currentMana = FMath::Clamp(currentMana + deltaMana, 0.0f, maxMana);
//}
//
//float AUMana::getCurrentMana() 
//{
//	return currentMana;
//}
