// Fill out your copyright notice in the Description page of Project Settings.
#include "Projectile.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup a Static Mesh for the Projectile
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Component"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetWorldScale3D(FVector(5));


	// Load in the Model for the Static Mesh
	UStaticMesh* meshToUse = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/ShurikenModelMesh.ShurikenModelMesh'")));
	
	if (meshToUse && StaticMesh)
	{
		// Set the Mesh to Hold the Model - Shuriken Model
		StaticMesh->SetStaticMesh(meshToUse);
	}

	// Lets us know which projectiles are for which ability
	bIsUltimateProjectile = false;

	// Collision Setup 
	
	StaticMesh->SetCollisionProfileName("OverlapAllDynamic");
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Overlap);	
	// Set the Collision to be enabled for Query and Physics - Overlap is Originally just Query so we have to set this again to deal with Physics
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// Set the Collision to ignore the Player
	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);	

	
}

// Called when the game starts or when spawned	
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// Setup how the Static Mesh will INteract with the world
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetEnableGravity(false);
	// Using Add Dynamic to bind the OnOverlap Function to the OnComponentBeginOverlap event of the Static Mesh 
	// This will call the OnOverlap Function every time the Mesh Overlaps with another Mesh
	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlap);

}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsUltimateProjectile && tempBool)
	{
		// Get the Players world Position
		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		// Set a Variable to hold the calculation between the Projectiles Location and the Player Location
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerLocation);

		// If this Distance is bigger than the one Specified
		if (DistanceToPlayer >= StandstillDistance)
		{
			// Stop the linear velocity but keep the angular velocity - this makes them stop in place and spin 
			StaticMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
			// Set the Temp Boolean to false;
			tempBool = false;
		}

	}
}

void AProjectile::getAngleRotation()
{
	// Adding a rotation on the Z axis using Angular Velocity
	StaticMesh->SetPhysicsAngularVelocityInDegrees(FVector(0.0f, 0.0f, 400.0f), false, NAME_None);
}



void AProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	UE_LOG(LogTemp, Warning, TEXT("COLLISION with %s"), *OtherActor->GetName());

	// If the projectile comes into contact with another shuriken - return (Don't do anything)
	if (AProjectile* projectile = Cast<AProjectile>(OtherActor))
	{
		return;
	}
	// If the Projectile Hits anything else - Destroy it 
	OtherActor->Destroy();

}
