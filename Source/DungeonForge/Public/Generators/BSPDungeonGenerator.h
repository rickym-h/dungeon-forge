// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleGridDungeonGenerator.h"
#include "BSPDungeonGenerator.generated.h"

/**
 * A simple structure to define a rectilinear box.
 * The box origin represents the top-left corner of the box.
 * The box bound represents the bottom-right corner of the box. 
 */
USTRUCT()
struct FRectBox
{
	GENERATED_BODY()

	FRectBox();
	FRectBox(const FGridCoordinate InBoxOrigin, const FGridCoordinate InBoxBound);
	
	FGridCoordinate BoxOrigin;
	FGridCoordinate BoxBound;

	int32 GetVolume() const;

	TArray<FGridCoordinate> GetFillCoordinates() const;
};

/**
 * Generates a dungeon layout using the Binary Space Partitioning (BSP) algorithm.
 */
UCLASS()
class DUNGEONFORGE_API UBSPDungeonGenerator : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	 * 
	 */
	UFUNCTION(BlueprintCallable)
	USimpleGridDungeonLayout* GenerateLayout();

protected:
	static TArray<TArray<FRectBox>> GetAllPossibleBoxSplits(const FRectBox& Box, int32 CorridorLength, int32 MinRoomWidth);
	static TArray<TArray<FRectBox>> GetTwoRandomPossibleBoxSplits(const FRectBox& Box, const int32 CorridorLength, const int32 MinRoomWidth);
};
