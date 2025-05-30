// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/SimpleGridDungeonInstance.h"

#include "Generators/SimpleGridDungeonGenerator.h"
#include "Layouts/SimpleGridDungeonLayout.h"


// Sets default values
ASimpleGridDungeonInstance::ASimpleGridDungeonInstance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Generator = CreateDefaultSubobject<USimpleGridDungeonGenerator>("Dungeon Generator");
	Layout = CreateDefaultSubobject<USimpleGridDungeonLayout>("Dungeon Layout");
}

void ASimpleGridDungeonInstance::GenerateLayout()
{
	FDateTime StartTime = FDateTime::UtcNow();
	Generator->SetNumRooms(RoomCount);
	float TimeElapsedInMs = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	UE_LOG(LogTemp, Display, TEXT("Initialised generator in %fms"), TimeElapsedInMs)
	
	StartTime = FDateTime::UtcNow();
	Layout = Generator->GenerateLayout();
	TimeElapsedInMs = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	UE_LOG(LogTemp, Display, TEXT("Generated layout in %fms"), TimeElapsedInMs)
}

void ASimpleGridDungeonInstance::SpawnDungeon()
{
	// Spawn all floor tiles
	SpawnRoomFloorTiles();
	SpawnCorridorFloorTiles();
	SpawnWallTiles();
	SpawnDoorTiles();
}

void ASimpleGridDungeonInstance::GenerateDungeon()
{
	UE_LOG(LogTemp, Log, TEXT("ASimpleGridDungeonInstance::GenerateDungeon()"));

	ClearDungeon();
	GenerateLayout();
	SpawnDungeon();
}

void ASimpleGridDungeonInstance::ClearDungeon()
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
void ASimpleGridDungeonInstance::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASimpleGridDungeonInstance::SpawnRoomFloorTiles()
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

void ASimpleGridDungeonInstance::SpawnCorridorFloorTiles()
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

void ASimpleGridDungeonInstance::SpawnWallTiles()
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

void ASimpleGridDungeonInstance::SpawnDoorTiles()
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

FVector ASimpleGridDungeonInstance::GetPositionForCoordinate(const FGridCoordinate& Coordinate) const
{
	return GetActorLocation() + UGridCoordinateHelperLibrary::GetWorldPositionFromGridCoordinate(Coordinate, GridSize);
}

FVector ASimpleGridDungeonInstance::GetPositionForEdge(const FGridEdge& Edge) const
{
	return GetActorLocation() + ((UGridCoordinateHelperLibrary::GetWorldPositionFromGridCoordinate(Edge.CoordinateA, GridSize) + UGridCoordinateHelperLibrary::GetWorldPositionFromGridCoordinate(Edge.CoordinateB, GridSize)) / 2.0f);
}

FRotator ASimpleGridDungeonInstance::GetRotationForEdge(const FGridEdge& Edge) const
{
	return (GetPositionForCoordinate(Edge.CoordinateB) - GetPositionForCoordinate(Edge.CoordinateA)).Rotation();
}