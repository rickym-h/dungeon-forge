// Fill out your copyright notice in the Description page of Project Settings.


#include "Layouts/GridCoordinateHelperLibrary.h"

FGridCoordinate::FGridCoordinate(): X(0), Y(0)
{
}

FGridCoordinate::FGridCoordinate(const int32 InX, const int32 InY)
{
	X = InX;
	Y = InY;
}

float FGridCoordinate::DistanceFromCentre() const
{
	if (X == 0 && Y == 0)
	{
		return 0.0f;
	} else if (X == 0)
	{
		return Y;
	} else if (Y == 0)
	{
		return X;
	}
	return FMath::Sqrt(static_cast<float>(FMath::Square(X) + FMath::Square(Y)));
}

FGridEdge::FGridEdge()
{
}

FGridEdge::FGridEdge(const FGridCoordinate& InCoordinateA, const FGridCoordinate& InCoordinateB, const bool bInDirected)
{
	CoordinateA = InCoordinateA;
	CoordinateB = InCoordinateB;
	bDirected = bInDirected;
}

FGridEdge FGridEdge::Sorted() const
{
	if (this->bDirected)
	{
		return *this; // Since coordinate is directed, do not override order.
	}
	if (GetTypeHash(CoordinateA) < GetTypeHash(CoordinateB))
	{
		return *this; // CoordinateA is less than CoordinateB, return as is.
	}
	else
	{
		return FGridEdge(CoordinateB, CoordinateA, false); // Swap coordinates to maintain order.
	}
}

TArray<FGridCoordinate> UGridCoordinateHelperLibrary::GetAdjacentCoordinates(const FGridCoordinate& Coordinate, const bool bIncludeDiagonal)
{
	TArray<FGridCoordinate> AdjacentCoordinates;

	// Add orthogonal coordinates
	AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X + 1, Coordinate.Y));
	AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X - 1, Coordinate.Y));
	AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X, Coordinate.Y + 1));
	AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X, Coordinate.Y - 1));

	if (bIncludeDiagonal)
	{
		// Add diagonal coordinates
		AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X + 1, Coordinate.Y + 1));
		AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X + 1, Coordinate.Y - 1));
		AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X - 1, Coordinate.Y + 1));
		AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X - 1, Coordinate.Y - 1));
	}

	return AdjacentCoordinates;
}

FVector UGridCoordinateHelperLibrary::GetWorldPositionFromGridCoordinate(const FGridCoordinate& Coordinate, const float TileSize)
{
	return FVector(Coordinate.X * TileSize, Coordinate.Y * TileSize, 0.0f);
}

TSet<FGridCoordinate> UGridCoordinateHelperLibrary::GetAllCoordinates(const TArray<FGridTile>& Tiles)
{
	TSet<FGridCoordinate> AllCoordinates;
	for (const FGridTile& Tile : Tiles)
	{
		AllCoordinates.Add(Tile.Coordinate);
	}
	return AllCoordinates;
}

TSet<FGridCoordinate> UGridCoordinateHelperLibrary::GetAllCoordinates(const TSet<FGridTile>& Tiles)
{
	return GetAllCoordinates(Tiles.Array());
}
