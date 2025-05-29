// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridCoordinateHelperLibrary.h"
#include "UObject/Object.h"
#include "SimpleGridDungeonLayout.generated.h"

/**
 * A simple dungeon layout, composed of just tiles and doors.
 */
UCLASS()
class DUNGEONFORGE_API USimpleGridDungeonLayout : public UObject
{
	GENERATED_BODY()

public:
	USimpleGridDungeonLayout();

	UFUNCTION(BlueprintCallable, Category = "Layout Data")
	TArray<FGridCoordinate> GetRoomTiles() const;
	
	UFUNCTION(BlueprintCallable, Category = "Layout Data")
	TArray<FGridCoordinate> GetCorridorTiles() const;
	
	/**
	 * Gets all tiles, including both room and corridor tiles.
	 */
	UFUNCTION(BlueprintCallable, Category = "Layout Data")
	TArray<FGridCoordinate> GetAllFloorTiles() const;
	
	UFUNCTION(BlueprintCallable, Category = "Layout Data")
	TArray<FGridEdge> GetDoorPositions(const float GridSize) const;
	
	/**
	 * Imputes the wall positioning based on the room and corridor tiles. Excludes door positions.
	 */
	UFUNCTION(BlueprintCallable, Category = "Layout Data")
	TArray<FGridEdge> GetWallPositions(const float GridSize) const;

	UFUNCTION()
	void AddRoomTiles(const TArray<FGridCoordinate>& InRoomTiles);
	UFUNCTION()
	void AddCorridorTiles(const TArray<FGridCoordinate>& InCorridorTiles);
	UFUNCTION()
	void AddDoors(const TArray<FGridEdge>& InDoorLocations);

protected:
	TSet<FGridCoordinate> RoomTiles;
	TSet<FGridCoordinate> CorridorTiles;
	TSet<FGridEdge> Doors;
};
