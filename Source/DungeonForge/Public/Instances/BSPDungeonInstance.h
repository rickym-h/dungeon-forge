// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDungeonInstance.h"
#include "GameFramework/Actor.h"
#include "BSPDungeonInstance.generated.h"

struct FGridEdge;
struct FGridCoordinate;
class USimpleGridDungeonLayout;
class UBSPDungeonGenerator;

UCLASS()
class DUNGEONFORGE_API ABSPDungeonInstance : public ABaseDungeonInstance
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABSPDungeonInstance();

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
	UBSPDungeonGenerator* Generator;
	UPROPERTY()
	USimpleGridDungeonLayout* Layout;
	
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
