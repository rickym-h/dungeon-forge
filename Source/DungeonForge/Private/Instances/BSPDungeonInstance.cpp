// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/BSPDungeonInstance.h"

#include "Generators/BSPDungeonGenerator.h"
#include "Layouts/SimpleGridDungeonLayout.h"


// Sets default values
ABSPDungeonInstance::ABSPDungeonInstance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Generator = CreateDefaultSubobject<UBSPDungeonGenerator>("Dungeon Generator");
	Layout = CreateDefaultSubobject<USimpleGridDungeonLayout>("Dungeon Layout");
}

void ABSPDungeonInstance::GenerateLayout()
{
	Layout = Generator->GenerateLayout();
}

void ABSPDungeonInstance::SpawnDungeon()
{
	// Spawn all floor tiles
	const FDateTime StartT = FDateTime::UtcNow();
	SpawnRoomFloorTiles();
	const FDateTime SpawnedRoomFloorTiles = FDateTime::UtcNow();
	UE_LOG(LogTemp, Log, TEXT("Spawned Room Floor Tiles in %f milliseconds"), (SpawnedRoomFloorTiles - StartT).GetTotalMilliseconds());
	SpawnCorridorFloorTiles();
	const FDateTime SpawnedCorridorFloorTiles = FDateTime::UtcNow();
	UE_LOG(LogTemp, Log, TEXT("Spawned Corridor Floor Tiles in %f milliseconds"), (SpawnedCorridorFloorTiles - SpawnedRoomFloorTiles).GetTotalMilliseconds());
	SpawnWallTiles();
	const FDateTime SpawnedWallTiles = FDateTime::UtcNow();
	UE_LOG(LogTemp, Log, TEXT("Spawned Wall Tiles in %f milliseconds"), (SpawnedWallTiles - SpawnedCorridorFloorTiles).GetTotalMilliseconds());
	SpawnDoorTiles();
	const FDateTime SpawnedDoorTiles = FDateTime::UtcNow();
	UE_LOG(LogTemp, Log, TEXT("Spawned Door Tiles in %f milliseconds"), (SpawnedDoorTiles - SpawnedWallTiles).GetTotalMilliseconds());
}

void ABSPDungeonInstance::GenerateDungeon()
{
	UE_LOG(LogTemp, Log, TEXT("ABSPDungeonInstance::GenerateDungeon()"));

	ClearDungeon();
	GenerateLayout();
	SpawnDungeon();
}

void ABSPDungeonInstance::ClearDungeon()
{
	while (!RoomFloorMeshes.IsEmpty())
	{
		if (UStaticMeshComponent* Mesh = RoomFloorMeshes.Pop())
		{
			Mesh->DestroyComponent();
		}
	}
	while (!CorridorFloorMeshes.IsEmpty())
	{
		if (UStaticMeshComponent* Mesh = CorridorFloorMeshes.Pop())
		{
			Mesh->DestroyComponent();
		}
	}
	while (!WallMeshes.IsEmpty())
	{
		if (UStaticMeshComponent* Mesh = WallMeshes.Pop())
		{
			Mesh->DestroyComponent();
		}
	}
	while (!DoorMeshes.IsEmpty())
	{
		if (UStaticMeshComponent* Mesh = DoorMeshes.Pop())
		{
			Mesh->DestroyComponent();
		}
	}
}

// Called when the game starts or when spawned
void ABSPDungeonInstance::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABSPDungeonInstance::SpawnRoomFloorTiles()
{
	for (const FGridCoordinate& Coordinate : Layout->GetRoomTiles())
	{
		// Spawn floor tile
		if (UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this))
		{
			MeshComponent->SetStaticMesh(RoomFloorMesh);
			MeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			
			const FTransform TileTransform = FTransform(
				FRotator::ZeroRotator,
				GetPositionForCoordinate(Coordinate),
				FVector(1.0f, 1.0f, 1.0f));
			MeshComponent->SetWorldTransform(TileTransform);
			MeshComponent->RegisterComponent();
			
			// Add to the persistent array
			RoomFloorMeshes.Add(MeshComponent);
		}
	}
}

void ABSPDungeonInstance::SpawnCorridorFloorTiles()
{
	for (const FGridCoordinate& Coordinate : Layout->GetCorridorTiles())
	{
		// Spawn floor tile
		if (UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this))
		{
			MeshComponent->SetStaticMesh(CorridorFloorMesh);
			MeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			
			const FTransform TileTransform = FTransform(
				FRotator::ZeroRotator,
				GetPositionForCoordinate(Coordinate),
				FVector(1.0f, 1.0f, 1.0f));
			
			MeshComponent->SetWorldTransform(TileTransform);
			MeshComponent->RegisterComponent();
			
			// Add to the persistent array
			CorridorFloorMeshes.Add(MeshComponent);
		}
	}
}

void ABSPDungeonInstance::SpawnWallTiles()
{
	for (const FGridEdge& Edge : Layout->GetWallPositions(GridSize))
	{
		// Spawn wall tile
		if (UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this))
		{
			MeshComponent->SetStaticMesh(WallMesh);
			MeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			
			const FTransform EdgeTransform = FTransform(
				GetRotationForEdge(Edge),
				GetPositionForEdge(Edge),
				FVector(1.0f, 1.0f, 1.0f)
				);
			
			MeshComponent->SetWorldTransform(EdgeTransform);
			MeshComponent->RegisterComponent();
			
			// Add to the persistent array
			WallMeshes.Add(MeshComponent);
		}
	}
}

void ABSPDungeonInstance::SpawnDoorTiles()
{
	for (const FGridEdge& Edge : Layout->GetDoorPositions(GridSize))
	{
		// Spawn door tile
		if (UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this))
		{
			MeshComponent->SetStaticMesh(DoorMesh);
			MeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			
			const FTransform EdgeTransform = FTransform(
				GetRotationForEdge(Edge),
				GetPositionForEdge(Edge),
				FVector(1.0f, 1.0f, 1.0f)
				);
			
			MeshComponent->SetWorldTransform(EdgeTransform);
			MeshComponent->RegisterComponent();
			
			// Add to the persistent array
			DoorMeshes.Add(MeshComponent);
		}
	}
}

FVector ABSPDungeonInstance::GetPositionForCoordinate(const FGridCoordinate& Coordinate) const
{
	return GetActorLocation() + UGridCoordinateHelperLibrary::GetWorldPositionFromGridCoordinate(Coordinate, GridSize);
}

FVector ABSPDungeonInstance::GetPositionForEdge(const FGridEdge& Edge) const
{
	return GetActorLocation() + ((UGridCoordinateHelperLibrary::GetWorldPositionFromGridCoordinate(Edge.CoordinateA, GridSize) + UGridCoordinateHelperLibrary::GetWorldPositionFromGridCoordinate(Edge.CoordinateB, GridSize)) / 2.0f);
}

FRotator ABSPDungeonInstance::GetRotationForEdge(const FGridEdge& Edge) const
{
	return (GetPositionForCoordinate(Edge.CoordinateB) - GetPositionForCoordinate(Edge.CoordinateA)).Rotation();
}
