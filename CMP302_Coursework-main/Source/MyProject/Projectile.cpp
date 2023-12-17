// Fill out your copyright notice in the Description page of Project Settings.
#include "Projectile.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Component"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetWorldScale3D(FVector(5));
	
	UStaticMesh* meshToUse = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/ShurikenModelMesh.ShurikenModelMesh'")));
	

	if (meshToUse && StaticMesh)
	{
		StaticMesh->SetStaticMesh(meshToUse);
	}

	bIsUltimateProjectile = false;

	StaticMesh->SetCollisionProfileName("OverlapAllDynamic");
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
}

// Called when the game starts or when spawned	
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetEnableGravity(false);
	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlap);

}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsUltimateProjectile && tempBool)
	{
		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerLocation);

		if (DistanceToPlayer >= StandstillDistance)
		{
			// Cut linear velocity but keep angular velocity (rotation)
			StaticMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
			tempBool = false;
		}

	}
	//hasHitPlayer();
}

void AProjectile::hasHitPlayer()
{
	if (this->GetActorLocation() == GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() && !tempBool)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("HITTING!")));
		this->Destroy();
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
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("COLLLLISSSIOOONNNNNNN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")));

	if (AProjectile* projectile = Cast<AProjectile>(OtherActor))
	{
		return;
	}
	OtherActor->Destroy();

}
