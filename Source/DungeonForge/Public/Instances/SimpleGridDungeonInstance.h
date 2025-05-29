// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDungeonInstance.h"
#include "Layouts/GridCoordinateHelperLibrary.h"
#include "SimpleGridDungeonInstance.generated.h"

class USimpleGridDungeonGenerator;
class USimpleGridDungeonLayout;

UCLASS()
class DUNGEONFORGE_API ASimpleGridDungeonInstance : public ABaseDungeonInstance
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASimpleGridDungeonInstance();

	virtual void GenerateLayout() override;
	virtual void SpawnDungeon() override;
	UFUNCTION(Category="Generator Functions", CallInEditor)	
	virtual void GenerateDungeon() override;
	UFUNCTION(Category="Generator Functions", CallInEditor)
	virtual void ClearDungeon() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	USimpleGridDungeonGenerator* Generator;
	UPROPERTY()
	USimpleGridDungeonLayout* Layout;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Settings", meta=(ClampMin=1, ClampMax=100))
	int32 RoomCount = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Settings|Corridors")
	bool bAllowCorridors = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Settings|Corridors", meta=(EditCondition="bAllowCorridors", ClampMin=1, ClampMax=20))
	int32 CorridorLength = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings|Static Meshes")
	UStaticMesh* RoomFloorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings|Static Meshes")
	UStaticMesh* CorridorFloorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings|Static Meshes")
	UStaticMesh* WallMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings|Static Meshes")
	UStaticMesh* DoorMesh;

	UPROPERTY(VisibleAnywhere, Category = "Static Meshes")
	TArray<UStaticMeshComponent*> RoomFloorMeshes;
	UPROPERTY(VisibleAnywhere, Category = "Static Meshes")
	TArray<UStaticMeshComponent*> CorridorFloorMeshes;
	UPROPERTY(VisibleAnywhere, Category = "Static Meshes")
	TArray<UStaticMeshComponent*> WallMeshes;
	UPROPERTY(VisibleAnywhere, Category = "Static Meshes")
	TArray<UStaticMeshComponent*> DoorMeshes;
	
	void SpawnRoomFloorTiles();
	void SpawnCorridorFloorTiles();
	void SpawnWallTiles();
	void SpawnDoorTiles();

	FVector GetPositionForCoordinate(const FGridCoordinate& Coordinate) const;
	FVector GetPositionForEdge(const FGridEdge& Edge) const;
	FRotator GetRotationForEdge(const FGridEdge& Edge) const;
};
