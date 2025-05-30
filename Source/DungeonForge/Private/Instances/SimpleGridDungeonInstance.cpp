// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/SimpleGridDungeonInstance.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Generators/SimpleGridDungeonGenerator.h"
#include "Layouts/SimpleGridDungeonLayout.h"


// Sets default values
ASimpleGridDungeonInstance::ASimpleGridDungeonInstance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Generator = CreateDefaultSubobject<USimpleGridDungeonGenerator>("Dungeon Generator");
	Layout = CreateDefaultSubobject<USimpleGridDungeonLayout>("Dungeon Layout");
	
	RoomFloorMeshISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>("FloorMeshISM");
	RoomFloorMeshISM->SetupAttachment(RootComponent);
	CorridorFloorMeshISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>("CorridorFloorMeshISM");
	CorridorFloorMeshISM->SetupAttachment(RootComponent);
	WallMeshISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>("WallMeshISM");
	WallMeshISM->SetupAttachment(RootComponent);
	DoorMeshISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>("DoorMeshISM");
	DoorMeshISM->SetupAttachment(RootComponent);
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

	FDateTime StartT = FDateTime::UtcNow();
	ClearDungeon();
	GenerateLayout();
	SpawnDungeon();
	FDateTime FinishT = FDateTime::UtcNow();
	UE_LOG(LogTemp, Log, TEXT("Generated Dungeon in %f milliseconds"), (FinishT - StartT).GetTotalMilliseconds());
}

void ASimpleGridDungeonInstance::ClearDungeon()
{
	RoomFloorMeshISM->ClearInstances();
	CorridorFloorMeshISM->ClearInstances();
	WallMeshISM->ClearInstances();
	DoorMeshISM->ClearInstances();
}

// Called when the game starts or when spawned
void ASimpleGridDungeonInstance::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASimpleGridDungeonInstance::SpawnRoomFloorTiles()
{
	TArray<FTransform> RoomFloorTransforms;
	for (const FGridCoordinate& Coordinate : Layout->GetRoomTiles())
	{
		const FTransform TileTransform = FTransform(
			FRotator::ZeroRotator,
			GetPositionForCoordinate(Coordinate),
			FVector(1.0f, 1.0f, 1.0f));
		
		RoomFloorTransforms.Add(TileTransform);
	}
	RoomFloorMeshISM->AddInstances(RoomFloorTransforms, false);
	RoomFloorMeshISM->SetStaticMesh(RoomFloorMesh);
}

void ASimpleGridDungeonInstance::SpawnCorridorFloorTiles()
{
	TArray<FTransform> CorridorFloorTransforms;
	for (const FGridCoordinate& Coordinate : Layout->GetCorridorTiles())
	{
		const FTransform TileTransform = FTransform(
			FRotator::ZeroRotator,
			GetPositionForCoordinate(Coordinate),
			FVector(1.0f, 1.0f, 1.0f));
		
		CorridorFloorTransforms.Add(TileTransform);
	}
	CorridorFloorMeshISM->AddInstances(CorridorFloorTransforms, false);
	CorridorFloorMeshISM->SetStaticMesh(CorridorFloorMesh);
}

void ASimpleGridDungeonInstance::SpawnWallTiles()
{
	TArray<FTransform> WallTransforms;
	for (const FGridEdge& Edge : Layout->GetWallPositions(GridSize))
	{
		const FTransform EdgeTransform = FTransform(
			GetRotationForEdge(Edge),
			GetPositionForEdge(Edge),
			FVector(1.0f, 1.0f, 1.0f)
			);
		
		WallTransforms.Add(EdgeTransform);
	}
	WallMeshISM->AddInstances(WallTransforms, false);
	WallMeshISM->SetStaticMesh(WallMesh);
}

void ASimpleGridDungeonInstance::SpawnDoorTiles()
{
	TArray<FTransform> DoorTransforms;
	for (const FGridEdge& Edge : Layout->GetDoorPositions(GridSize))
	{
		const FTransform EdgeTransform = FTransform(
			GetRotationForEdge(Edge),
			GetPositionForEdge(Edge),
			FVector(1.0f, 1.0f, 1.0f)
			);
		
		DoorTransforms.Add(EdgeTransform);
	}
	DoorMeshISM->AddInstances(DoorTransforms, false);
	DoorMeshISM->SetStaticMesh(DoorMesh);
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