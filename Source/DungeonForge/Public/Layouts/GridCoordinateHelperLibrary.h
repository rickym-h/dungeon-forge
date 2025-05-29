// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GridCoordinateHelperLibrary.generated.h"


/**
 * A simple grid coordinate system for dungeons.
 */
USTRUCT(BlueprintType)
struct FGridCoordinate
{
	GENERATED_BODY()
	
	FGridCoordinate();
	FGridCoordinate(const int32 InX, const int32 InY);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Coordinate")
	int32 X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Coordinate")
	int32 Y;

	bool operator==(const FGridCoordinate& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	bool operator!=(const FGridCoordinate& Other) const
	{
		return !(*this == Other);
	}

	float DistanceFromCentre() const;
};

/**
 * @param Coordinate 
 * @return Generates hash for the grid coordinate. Required for TSet and TMap usage.
 */
FORCEINLINE uint32 GetTypeHash(const FGridCoordinate& Coordinate)
{
	return FCrc::MemCrc32(&Coordinate, sizeof(Coordinate));
}

/**
 * A data structure representing an edge in a grid layout. Can be used to represent doors, walls, or other connections between tiles.
 */
USTRUCT(BlueprintType)
struct FGridEdge
{
	GENERATED_BODY()
	
	FGridEdge();
	FGridEdge(const FGridCoordinate& InCoordinateA, const FGridCoordinate& InCoordinateB, const bool bInDirected);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoordinate CoordinateA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoordinate CoordinateB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDirected = false; // If true, the edge is directed from CoordinateA to CoordinateB
	
	/**
	 * If directed, the edge is considered to go from CoordinateA to CoordinateB. Otherwise edge is considered identical to another edge with A and B swapped.
	 */
	bool operator==(const FGridEdge& Other) const
	{
		if (bDirected)
		{
			return CoordinateA == Other.CoordinateA && CoordinateB == Other.CoordinateB;
		}
		else
		{
			return (CoordinateA == Other.CoordinateA && CoordinateB == Other.CoordinateB) ||
				   (CoordinateA == Other.CoordinateB && CoordinateB == Other.CoordinateA);
		}
	}

	bool operator!=(const FGridEdge& Other) const
	{
		return !(*this == Other);
	}

	/**
	 * 
	 * @return Returns an arbitrary sorted version of the edge, where CoordinateA is considered less than CoordinateB if undirected.
	 * Useful for comparisons with undirected edges if CoordinateA and CoordinateB are swapped.
	 */
	FGridEdge Sorted() const;
};

/**
 * @param Edge 
 * @return Generates hash for the grid edge. Required for TSet and TMap usage.
 */
FORCEINLINE uint32 GetTypeHash(const FGridEdge& Edge)
{
	if (Edge.bDirected)
	{
		return FCrc::MemCrc32(&Edge, sizeof(Edge));
	}
	else
	{
		// If edge is undirected, get the sorted version to ensure edges are the same regardless of order.
		const FGridEdge Temp = Edge.Sorted();
		return FCrc::MemCrc32(&Temp, sizeof(Temp));
	}
}

/**
 * A data structure representing a tile in a grid layout.
 */
USTRUCT(BlueprintType)
struct FGridTile
{
	GENERATED_BODY()

	FGridTile();
	FGridTile(FGridCoordinate InCoordinate, int32 InSize);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Coordinate")
	FGridCoordinate Coordinate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Coordinate")
	int32 Size;

	bool operator==(const FGridTile& Other) const
	{
		return Coordinate == Other.Coordinate && Size == Other.Size;
	}

	bool operator!=(const FGridTile& Other) const
	{
		return !(*this == Other);
	}
};

/**
 * @param GridTile 
 * @return Generates hash for the grid coordinate. Required for TSet and TMap usage.
 */
FORCEINLINE uint32 GetTypeHash(const FGridTile& GridTile)
{
	return FCrc::MemCrc32(&GridTile, sizeof(GridTile));
}

/**
 * A library of utility functions for working with grid coordinates in dungeon systems.
 */
UCLASS()
class DUNGEONFORGE_API UGridCoordinateHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static TArray<FGridCoordinate> GetAdjacentCoordinates(const FGridCoordinate& Coordinate, const bool bIncludeDiagonal = false);
	
	UFUNCTION(BlueprintPure)
	static FVector GetWorldPositionFromGridCoordinate(const FGridCoordinate& Coordinate, const float TileSize = 100.0f);
	UFUNCTION(BlueprintPure)
	static FVector GetWorldPositionFromGridTile(const FGridTile& Tile);

	UFUNCTION(BlueprintPure)
	static TSet<FGridCoordinate> GetAllCoordinates(const TArray<FGridTile>& Tiles);
	static TSet<FGridCoordinate> GetAllCoordinates(const TSet<FGridTile>& Tiles);
	
};
