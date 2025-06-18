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
	
	FGridCoordinate operator+(const FGridCoordinate& GlobalCentre) const
	{
		const FGridCoordinate Temp = FGridCoordinate(X + GlobalCentre.X, Y + GlobalCentre.Y);
		return Temp;
	}

	float DistanceFromCentre() const;
	FGridCoordinate Inverse() const;
	
	FGridCoordinate Rotate(int32 RotationCount) const;
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
	FGridEdge(const FGridCoordinate& InCoordinateA, const FGridCoordinate& InCoordinateB);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoordinate CoordinateA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoordinate CoordinateB;
	
	/**
	 * If directed, the edge is considered to go from CoordinateA to CoordinateB. Otherwise edge is considered identical to another edge with A and B swapped.
	 */
	bool operator==(const FGridEdge& Other) const
	{
		return (CoordinateA == Other.CoordinateA && CoordinateB == Other.CoordinateB) ||
			   (CoordinateA == Other.CoordinateB && CoordinateB == Other.CoordinateA);
	}

	bool operator!=(const FGridEdge& Other) const
	{
		return !(*this == Other);
	}

	/**
	 * 
	 * @return Creates an arbitrary sorted version of the edge, where CoordinateA is considered less than CoordinateB if undirected.
	 * Useful for comparisons with undirected edges if CoordinateA and CoordinateB are swapped.
	 */
	void Sort();
	
	bool SharesSingleCoordinate(FGridEdge EdgeB, FGridCoordinate& OutSharedCoordinate) const;
	bool FormsCorner(FGridEdge EdgeB) const;
};

/**
 * @param Edge 
 * @return Generates hash for the grid edge. Required for TSet and TMap usage.
 */
FORCEINLINE uint32 GetTypeHash(const FGridEdge& Edge)
{
	return FCrc::MemCrc32(&Edge, sizeof(Edge));
}

/**
 * A data structure representing an edge in a grid layout. Can be used to represent doors, walls, or other connections between tiles.
 */
USTRUCT(BlueprintType)
struct FGridCorner
{
	GENERATED_BODY()
	
	FGridCorner();
	FGridCorner(const FGridCoordinate& InCoordinateA, const FGridCoordinate& InCoordinateB, const FGridCoordinate& InCoordinateC, const FGridCoordinate& InCoordinateD);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoordinate CoordinateA;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoordinate CoordinateB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoordinate CoordinateC;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoordinate CoordinateD;
	
	void Sort();
	
	bool operator==(const FGridCorner& Other) const
	{
		return (CoordinateA == Other.CoordinateA) && (CoordinateB == Other.CoordinateB) && (CoordinateC == Other.CoordinateC) && (CoordinateD == Other.CoordinateD);
	}

	bool operator!=(const FGridCorner& Other) const
	{
		return !(*this == Other);
	}

	/**
	 * 
	 * @param EdgeA 
	 * @param EdgeB 
	 * @return The grid corner formed by two perpendicular edges A and B. These edges should share a common tile.
	 */
	static FGridCorner FromEdges(const FGridEdge& EdgeA, const FGridEdge& EdgeB);
};

/**
 * @param Corner 
 * @return Generates hash for the grid corner. Required for TSet and TMap usage.
 */
FORCEINLINE uint32 GetTypeHash(const FGridCorner& Corner)
{
	return FCrc::MemCrc32(&Corner, sizeof(Corner));
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
	static TArray<FGridCoordinate> GetAdjacentCoordinates(const FGridCoordinate& Coordinate, const bool bIncludeDiagonal = false, int32 Direction = 0);
	
	UFUNCTION(BlueprintPure)
	static FVector GetWorldPositionFromGridCoordinate(const FGridCoordinate& Coordinate, const float TileSize = 100.0f);

	/**
	 * 
	 * @param RoomRepresentation An array of coordinates representing the room.
	 * @param ExpansionDistance The distance to expand the room by.
	 * @param bIncludeDiagonal Whether to include diagonal coordinates when expanding (square becomes a square vs square becomes a cross)
	 * @param Direction Specific a direction. 0 means every direction. 1 means only up, 2 means only right, 3 means only down, 4 means only left.
	 * @return The expanded room.
	*/
	UFUNCTION(BlueprintPure)
	static TArray<FGridCoordinate> Expand(TArray<FGridCoordinate> RoomRepresentation, const int32 ExpansionDistance, const bool bIncludeDiagonal = false, const int32 Direction = 0);

	
	UFUNCTION(BlueprintPure)
	static TSet<FGridCoordinate> RotateClockwise(const TSet<FGridCoordinate>& Coordinates, const int32 RotationCount = 1);
};
