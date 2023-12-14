#include "CubeActor.h"

// Sets default values
ACubeActor::ACubeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticCubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube Mesh"));
	UStaticMesh* cubeMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'")));
	StaticCubeMesh->SetStaticMesh(cubeMesh);
	RootComponent = StaticCubeMesh;
	StaticCubeMesh->SetWorldScale3D(FVector(0.3));

	// Collision

}

// Called when the game starts or when spawned
void ACubeActor::BeginPlay()
{
	Super::BeginPlay();
	
	

}

// Called every frame
void ACubeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

