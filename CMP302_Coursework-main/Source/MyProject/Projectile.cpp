// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere Component"));
	StaticMesh->SetWorldScale3D(FVector(10));
	
	
	//UStaticMesh* meshToUse = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_Torus.Shape_Torus'")));

	UStaticMesh* meshToUse = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/ShurikenModelMesh.ShurikenModelMesh'")));
	

	if (meshToUse && StaticMesh)
	{
		StaticMesh->SetStaticMesh(meshToUse);
	}


}

// Called when the game starts or when spawned	
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetEnableGravity(false);

}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);
}


void AProjectile::getAngleRotation()
{
	// Adding a rotation on the Z axis using Angular Velocity
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Its calling this function")));
	StaticMesh->SetPhysicsAngularVelocityInDegrees(FVector(0.0f, 0.0f, 400.0f), false, NAME_None);
}
