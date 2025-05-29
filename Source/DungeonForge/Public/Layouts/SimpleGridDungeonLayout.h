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
	TArray<FGridTile> GetRoomTiles() const;
	
	UFUNCTION(BlueprintCallable, Category = "Layout Data")
	TArray<FGridTile> GetCorridorTiles() const;
	
	/**
	 * Gets all tiles, including both room and corridor tiles.
	 */
	UFUNCTION(BlueprintCallable, Category = "Layout Data")
	TArray<FGridTile> GetAllFloorTiles() const;
	
	UFUNCTION(BlueprintCallable, Category = "Layout Data")
	TArray<FGridEdge> GetDoorPositions(const float GridSize) const;
	
	/**
	 * Imputes the wall positioning based on the room and corridor tiles. Excludes door positions.
	 */
	UFUNCTION(BlueprintCallable, Category = "Layout Data")
	TArray<FGridEdge> GetWallPositions(const float GridSize) const;

	UFUNCTION()
	void AddRoomTiles(const TArray<FGridTile>& InRoomTiles);
	UFUNCTION()
	void AddCorridorTiles(const TArray<FGridTile>& InCorridorTiles);
	UFUNCTION()
	void AddDoors(const TArray<FGridEdge>& InDoorLocations);

protected:
	TSet<FGridTile> RoomTiles;
	TSet<FGridTile> CorridorTiles;
	TSet<FGridEdge> Doors;
};
