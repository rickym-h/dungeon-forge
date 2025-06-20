// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDungeonInstance.h"
#include "Layouts/GridCoordinateHelperLibrary.h"
#include "SimpleGridDungeonInstance.generated.h"

class USimpleGridDungeonGenerator;
class USimpleGridDungeonLayout;

UCLASS(Blueprintable, BlueprintType)
class DUNGEONFORGE_API ASimpleGridDungeonInstance : public ABaseDungeonInstance
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASimpleGridDungeonInstance();

	virtual void GenerateLayout() override;
	virtual void SpawnDungeon() override;
	UFUNCTION(BlueprintCallable, Category="Generator Functions", CallInEditor)	
	virtual void GenerateDungeon() override;
	UFUNCTION(BlueprintCallable, Category="Generator Functions", CallInEditor)	
	virtual void ClearDungeon() override;

	UFUNCTION(BlueprintCallable, Category = "Post-Generation Helpers")
	TArray<FVector> GetRoomFloorPositions() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	USimpleGridDungeonGenerator* Generator;
	UPROPERTY()
	USimpleGridDungeonLayout* Layout;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Settings", meta=(ClampMin=1, ClampMax=1000))
	int32 RoomCount = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Settings|Corridors")
	bool bAllowCorridors = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Settings|Corridors", meta=(EditCondition="bAllowCorridors", ClampMin=1, ClampMax=20))
	int32 CorridorLength = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings|Static Meshes")
	UStaticMesh* RoomFloorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings|Static Meshes")
	bool bUseRandomFloorOrientation = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings|Static Meshes")
	UStaticMesh* CorridorFloorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings|Static Meshes")
	UStaticMesh* WallMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings|Static Meshes")
	UStaticMesh* DoorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings|Static Meshes")
	UStaticMesh* PillarMesh;

	UPROPERTY()
	UInstancedStaticMeshComponent* RoomFloorMeshISM;
	UPROPERTY()
	UInstancedStaticMeshComponent* CorridorFloorMeshISM;
	UPROPERTY()
	UInstancedStaticMeshComponent* WallMeshISM;
	UPROPERTY()
	UInstancedStaticMeshComponent* DoorMeshISM;
	UPROPERTY()
	UInstancedStaticMeshComponent* PillarMeshISM;

	void SpawnRoomFloorTiles();
	void SpawnCorridorFloorTiles();
	void SpawnWallTiles();
	void SpawnDoorTiles();
	void SpawnCornerPillars();

	FVector GetPositionForCoordinate(const FGridCoordinate& Coordinate) const;
	FVector GetPositionForCorner(const FGridCorner& Corner) const;
	FVector GetPositionForEdge(const FGridEdge& Edge) const;
	FRotator GetRotationForEdge(const FGridEdge& Edge) const;
};
