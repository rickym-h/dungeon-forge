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

FGridCoordinate FGridCoordinate::Inverse() const
{
	return FGridCoordinate(-this->X, -this->Y);
}

FGridCoordinate FGridCoordinate::Rotate(const int32 RotationCount) const
{
	if (RotationCount == 1)
	{
		return FGridCoordinate(this->Y, -this->X);
	}
	return FGridCoordinate(this->Y, -this->X).Rotate(RotationCount - 1);
}

FGridEdge::FGridEdge()
{
}

FGridEdge::FGridEdge(const FGridCoordinate& InCoordinateA, const FGridCoordinate& InCoordinateB)
{
	CoordinateA = InCoordinateA;
	CoordinateB = InCoordinateB;
	Sort();
}

void FGridEdge::Sort()
{
	if (GetTypeHash(CoordinateA) < GetTypeHash(CoordinateB)) { return; }

	// swap coordinates
	const FGridCoordinate Temp = CoordinateA;
	CoordinateA = CoordinateB;
	CoordinateB = Temp;
}

TArray<FGridCoordinate> UGridCoordinateHelperLibrary::GetAdjacentCoordinates(const FGridCoordinate& Coordinate, const bool bIncludeDiagonal, const int32 Direction)
{
	TArray<FGridCoordinate> AdjacentCoordinates;

	// Add orthogonal coordinates
	if (Direction == 0 || Direction == 2) AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X + 1, Coordinate.Y));
	if (Direction == 0 || Direction == 4) AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X - 1, Coordinate.Y));
	if (Direction == 0 || Direction == 1) AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X, Coordinate.Y + 1));
	if (Direction == 0 || Direction == 3) AdjacentCoordinates.Add(FGridCoordinate(Coordinate.X, Coordinate.Y - 1));

	if (bIncludeDiagonal && Direction == 0)
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

TArray<FGridCoordinate> UGridCoordinateHelperLibrary::Expand(TArray<FGridCoordinate> RoomRepresentation, const int32 ExpansionDistance,
	const bool bIncludeDiagonal, const int32 Direction)
{
	TSet<FGridCoordinate> RoomRepresentationSet = TSet(RoomRepresentation);

	for (int i = 0; i < ExpansionDistance; i++)
	{
		for (FGridCoordinate Coordinate : RoomRepresentation)
		{
			RoomRepresentationSet.Append(GetAdjacentCoordinates(Coordinate, bIncludeDiagonal, Direction));
		}
		RoomRepresentation = RoomRepresentationSet.Array();
	}
	
	return RoomRepresentationSet.Array();
}

TSet<FGridCoordinate> UGridCoordinateHelperLibrary::RotateClockwise(const TSet<FGridCoordinate>& Coordinates, const int32 RotationCount)
{
	TSet<FGridCoordinate> RotatedCoordinates;
	
	for (const FGridCoordinate Coordinate : Coordinates)
	{
		RotatedCoordinates.Add(Coordinate.Rotate(RotationCount));
	}
	
	return RotatedCoordinates;
}
